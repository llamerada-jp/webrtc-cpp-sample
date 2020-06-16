
#include <condition_variable>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

// Define the macros to fit compile environment.
// 環境に合わせてマクロを定義する。
//#define WEBRTC_ANDROID 1
//#define WEBRTC_IOS 1
//#define WEBRTC_LINUX 1
//#define WEBRTC_MAC 1
//#define WEBRTC_POSIX 1
//#define WEBRTC_WIN 1

// Header files related with WebRTC.
// WebRTC関連のヘッダ
#include <api/create_peerconnection_factory.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/thread.h>
#include <system_wrappers/include/field_trial.h>

#include "picojson/picojson.h"

struct Ice {
  std::string candidate;
  std::string sdp_mid;
  int sdp_mline_index;
};

class Connection {
 public:
  const std::string name;

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
  rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;

  std::function<void(const std::string &)> on_sdp;
  std::function<void()> on_accept_ice;
  std::function<void(const Ice &)> on_ice;
  std::function<void()> on_success;
  std::function<void(const std::string &)> on_message;

  // When the status of the DataChannel changes, determine if the connection is complete.
  // DataChannelのstateが変化したら、接続が完了したか確かめる。
  void on_state_change() {
    std::cout << "state: " << data_channel->state() << std::endl;
    if (data_channel->state() == webrtc::DataChannelInterface::kOpen && on_success) {
      on_success();
    }
  }

  // After the SDP is successfully created, it is set as a LocalDescription and displayed as a string to be passed to
  // the other party.
  // SDPの作成が成功したら、LocalDescriptionとして設定し、相手に渡す文字列として表示する。
  void on_success_csd(webrtc::SessionDescriptionInterface *desc) {
    peer_connection->SetLocalDescription(ssdo, desc);

    std::string sdp;
    desc->ToString(&sdp);
    on_sdp(sdp);
  }

  // Convert the got ICE.
  // 取得したICEを変換する。
  void on_ice_candidate(const webrtc::IceCandidateInterface *candidate) {
    Ice ice;
    candidate->ToString(&ice.candidate);
    ice.sdp_mid         = candidate->sdp_mid();
    ice.sdp_mline_index = candidate->sdp_mline_index();
    on_ice(ice);
  }

  class PCO : public webrtc::PeerConnectionObserver {
   private:
    Connection &parent;

   public:
    PCO(Connection &parent) : parent(parent) {
    }

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::SignalingChange(" << new_state << ")" << std::endl;
    };

    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::AddStream" << std::endl;
    };

    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::RemoveStream" << std::endl;
    };

    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::DataChannel(" << data_channel << ", " << parent.data_channel.get() << ")"
                << std::endl;
      // The request recipient gets a DataChannel instance in the onDataChannel event.
      // リクエスト受信側は、onDataChannelイベントでDataChannelインスタンスをもらう。
      parent.data_channel = data_channel;
      parent.data_channel->RegisterObserver(&parent.dco);
    };

    void OnRenegotiationNeeded() override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::RenegotiationNeeded" << std::endl;
    };

    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::IceConnectionChange(" << new_state << ")" << std::endl;
    };

    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::IceGatheringChange(" << new_state << ")" << std::endl;
    };

    void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "PeerConnectionObserver::IceCandidate" << std::endl;
      parent.on_ice_candidate(candidate);
    };
  };

  class DCO : public webrtc::DataChannelObserver {
   private:
    Connection &parent;

   public:
    DCO(Connection &parent) : parent(parent) {
    }

    void OnStateChange() override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "DataChannelObserver::StateChange" << std::endl;
      parent.on_state_change();
    };

    // Message receipt.
    // メッセージ受信。
    void OnMessage(const webrtc::DataBuffer &buffer) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "DataChannelObserver::Message" << std::endl;
      if (parent.on_message) {
        parent.on_message(std::string(buffer.data.data<char>(), buffer.data.size()));
      }
    };

    void OnBufferedAmountChange(uint64_t previous_amount) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "DataChannelObserver::BufferedAmountChange(" << previous_amount << ")" << std::endl;
    };
  };

  class CSDO : public webrtc::CreateSessionDescriptionObserver {
   private:
    Connection &parent;

   public:
    CSDO(Connection &parent) : parent(parent) {
    }

    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "CreateSessionDescriptionObserver::OnSuccess" << std::endl;
      parent.on_success_csd(desc);
    };

    void OnFailure(webrtc::RTCError error) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "CreateSessionDescriptionObserver::OnFailure" << std::endl
                << error.message() << std::endl;
    };
  };

  class SSDO : public webrtc::SetSessionDescriptionObserver {
   private:
    Connection &parent;

   public:
    SSDO(Connection &parent) : parent(parent) {
    }

    void OnSuccess() override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "SetSessionDescriptionObserver::OnSuccess" << std::endl;
      if (parent.on_accept_ice) {
        parent.on_accept_ice();
      }
    };

    void OnFailure(webrtc::RTCError error) override {
      std::cout << parent.name << ":" << std::this_thread::get_id() << ":"
                << "SetSessionDescriptionObserver::OnFailure" << std::endl
                << error.message() << std::endl;
    };
  };

  PCO pco;
  DCO dco;
  rtc::scoped_refptr<CSDO> csdo;
  rtc::scoped_refptr<SSDO> ssdo;

  Connection(const std::string &name_) :
      name(name_),
      pco(*this),
      dco(*this),
      csdo(new rtc::RefCountedObject<CSDO>(*this)),
      ssdo(new rtc::RefCountedObject<SSDO>(*this)) {
  }
};

class Wrapper {
 public:
  const std::string name;
  std::unique_ptr<rtc::Thread> network_thread;
  std::unique_ptr<rtc::Thread> worker_thread;
  std::unique_ptr<rtc::Thread> signaling_thread;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory;
  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  Connection connection;

  Wrapper(const std::string name_) : name(name_), connection(name_) {
  }

  void on_sdp(std::function<void(const std::string &)> f) {
    connection.on_sdp = f;
  }

  void on_accept_ice(std::function<void()> f) {
    connection.on_accept_ice = f;
  }

  void on_ice(std::function<void(const Ice &)> f) {
    connection.on_ice = f;
  }

  void on_success(std::function<void()> f) {
    connection.on_success = f;
  }

  void on_message(std::function<void(const std::string &)> f) {
    connection.on_message = f;
  }

  void init() {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "init Main thread" << std::endl;

    // Using Google's STUN server.
    // GoogleのSTUNサーバを利用。
    webrtc::PeerConnectionInterface::IceServer ice_server;
    ice_server.uri = "stun:stun.l.google.com:19302";
    configuration.servers.push_back(ice_server);

    network_thread = rtc::Thread::CreateWithSocketServer();
    network_thread->Start();
    worker_thread = rtc::Thread::Create();
    worker_thread->Start();
    signaling_thread = rtc::Thread::Create();
    signaling_thread->Start();
    webrtc::PeerConnectionFactoryDependencies dependencies;
    dependencies.network_thread   = network_thread.get();
    dependencies.worker_thread    = worker_thread.get();
    dependencies.signaling_thread = signaling_thread.get();
    peer_connection_factory       = webrtc::CreateModularPeerConnectionFactory(std::move(dependencies));

    if (peer_connection_factory.get() == nullptr) {
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreateModularPeerConnectionFactory." << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void create_offer_sdp() {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "create_offer_sdp" << std::endl;

    connection.peer_connection =
        peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &connection.pco);

    webrtc::DataChannelInit config;

    // Configuring DataChannel.
    // DataChannelの設定。
    connection.data_channel = connection.peer_connection->CreateDataChannel("data_channel", &config);
    connection.data_channel->RegisterObserver(&connection.dco);

    if (connection.peer_connection.get() == nullptr) {
      peer_connection_factory = nullptr;
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreatePeerConnection." << std::endl;
      exit(EXIT_FAILURE);
    }
    connection.peer_connection->CreateOffer(connection.csdo, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
  }

  void create_answer_sdp(const std::string &parameter) {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "create_answer_sdp" << std::endl;

    connection.peer_connection =
        peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, &connection.pco);

    if (connection.peer_connection.get() == nullptr) {
      peer_connection_factory = nullptr;
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreatePeerConnection." << std::endl;
      exit(EXIT_FAILURE);
    }
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface *session_description(
        webrtc::CreateSessionDescription("offer", parameter, &error));
    if (session_description == nullptr) {
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreateSessionDescription." << std::endl
                << error.line << std::endl
                << error.description << std::endl;
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Offer SDP:begin" << std::endl
                << parameter << std::endl
                << "Offer SDP:end" << std::endl;
      exit(EXIT_FAILURE);
    }
    connection.peer_connection->SetRemoteDescription(connection.ssdo, session_description);
    connection.peer_connection->CreateAnswer(connection.csdo, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
  }

  void push_reply_sdp(const std::string &parameter) {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "push_reply_sdp" << std::endl;

    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface *session_description(
        webrtc::CreateSessionDescription("answer", parameter, &error));
    if (session_description == nullptr) {
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreateSessionDescription." << std::endl
                << error.line << std::endl
                << error.description << std::endl;
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Answer SDP:begin" << std::endl
                << parameter << std::endl
                << "Answer SDP:end" << std::endl;
      exit(EXIT_FAILURE);
    }
    connection.peer_connection->SetRemoteDescription(connection.ssdo, session_description);
  }

  void push_ice(const Ice &ice_it) {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "push_ice" << std::endl;

    webrtc::SdpParseError err_sdp;
    webrtc::IceCandidateInterface *ice =
        CreateIceCandidate(ice_it.sdp_mid, ice_it.sdp_mline_index, ice_it.candidate, &err_sdp);
    if (!err_sdp.line.empty() && !err_sdp.description.empty()) {
      std::cout << name << ":" << std::this_thread::get_id() << ":"
                << "Error on CreateIceCandidate" << std::endl
                << err_sdp.line << std::endl
                << err_sdp.description << std::endl;
      exit(EXIT_FAILURE);
    }
    connection.peer_connection->AddIceCandidate(ice);
  }

  void send(const std::string &parameter) {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "send" << std::endl;

    webrtc::DataBuffer buffer(rtc::CopyOnWriteBuffer(parameter.c_str(), parameter.size()), true);
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "Send(" << connection.data_channel->state() << ")" << std::endl;
    connection.data_channel->Send(buffer);
  }

  void quit() {
    std::cout << name << ":" << std::this_thread::get_id() << ":"
              << "quit" << std::endl;

    // Close with the thread running.
    // スレッドが起動した状態でCloseする。
    connection.peer_connection->Close();
    connection.peer_connection = nullptr;
    connection.data_channel    = nullptr;
    peer_connection_factory    = nullptr;

    network_thread->Stop();
    worker_thread->Stop();
    signaling_thread->Stop();
  }
};

int main(int argc, char *argv[]) {
  webrtc::field_trial::InitFieldTrialsFromString("");
  rtc::InitializeSSL();

  Wrapper webrtc("");
  std::list<Ice> ice_list;

  std::string line;
  std::string command;
  std::string parameter;
  std::string sdp_type;
  bool is_cmd_mode = true;

  webrtc.on_ice([&](const Ice &ice) { ice_list.push_back(ice); });
  webrtc.on_message([&](const std::string &message) { std::cout << message << std::endl; });
  webrtc.on_sdp([&](const std::string &sdp) {
    std::cout << sdp_type << " sdp:begin" << std::endl << sdp << sdp_type << " sdp:end" << std::endl;
  });

  webrtc.init();

  while (std::getline(std::cin, line)) {
    if (is_cmd_mode) {
      if (line == "") {
        continue;

      } else if (line == "sdp1") {
        sdp_type = "Offer";
        webrtc.create_offer_sdp();

      } else if (line == "sdp2") {
        command     = "sdp2";
        is_cmd_mode = false;

      } else if (line == "sdp3") {
        command     = "sdp3";
        is_cmd_mode = false;

      } else if (line == "ice1") {
        picojson::array ice_a;
        for (auto &ice : ice_list) {
          picojson::object ice_o;
          ice_o.insert(std::make_pair("candidate", ice.candidate));
          ice_o.insert(std::make_pair("sdp_mid", ice.sdp_mid));
          ice_o.insert(std::make_pair("sdp_mline_index", static_cast<double>(ice.sdp_mline_index)));
          ice_a.push_back(picojson::value(ice_o));
        }
        std::cout << picojson::value(ice_a).serialize(true) << std::endl;
        ice_list.clear();

      } else if (line == "ice2") {
        command     = "ice2";
        is_cmd_mode = false;

      } else if (line == "send") {
        command     = "send";
        is_cmd_mode = false;

      } else if (line == "quit") {
        webrtc.quit();
        break;

      } else {
        std::cout << "?" << line << std::endl;
      }
    } else {
      if (line == ";") {
        if (command == "sdp2") {
          sdp_type = "Answer";
          webrtc.create_answer_sdp(parameter);

        } else if (command == "sdp3") {
          webrtc.push_reply_sdp(parameter);

        } else if (command == "ice2") {
          picojson::value v;
          std::string err = picojson::parse(v, parameter);
          if (!err.empty()) {
            std::cout << "Error on parse json : " << err << std::endl;
            parameter = "";
            continue;
          }
          for (auto &it : v.get<picojson::array>()) {
            picojson::object &ice_o = it.get<picojson::object>();
            Ice ice;
            ice.candidate       = ice_o.at("candidate").get<std::string>();
            ice.sdp_mid         = ice_o.at("sdp_mid").get<std::string>();
            ice.sdp_mline_index = static_cast<int>(ice_o.at("sdp_mline_index").get<double>());
            webrtc.push_ice(ice);
          }
        } else if (command == "send") {
          webrtc.send(parameter);
        }

        parameter   = "";
        is_cmd_mode = true;
      } else {
        parameter += line + "\n";
      }
    }
  }

  rtc::CleanupSSL();

  return 0;
}
