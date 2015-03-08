#include <gloox.h>

#include <connectionbase.h>
#include <connectiontcpbase.h>
#include <connectionlistener.h>
#include <messagehandler.h>
#include <registrationhandler.h>
#include <registration.h>
#include <message.h>
#include <client.h>


#include <cstdio>
#include <vector>

using namespace gloox;

static const char* USER_PREFIX = "deviceA_";
static const char* XMPP_SERVER = "xmppeval.cloudapp.net";
static const char* XMPP_DOMAIN = "city.gov";
static const char* DEFAULT_PASSWORD = "password";
static const int NUM_USERS = 100;

class XMPP_Client : public ConnectionListener, MessageHandler, RegistrationHandler
{
public:
  XMPP_Client(uint32_t id, const char* domain, const char* password, const char* server);
  ~XMPP_Client();

  void connect();
  bool needRegistration() const;
  void registerAccount();
  void loop();
  int getSocket() const;

  // ConnectionListener
  virtual bool onTLSConnect(const CertInfo& info) { return true; }
  virtual void onConnect();
  virtual void onDisconnect(ConnectionError e);

  // MessageHandler
  virtual void handleMessage(const Message& msg, MessageSession* session = 0);

  // RegistrationHandler
  virtual void handleRegistrationFields(const JID& from, int fields, std::string instructions);
  virtual void handleAlreadyRegistered(const JID& from){};
  virtual void handleRegistrationResult(const JID& from, RegistrationResult regResult);
  virtual void handleDataForm(const JID& from, const DataForm& form){};
  virtual void handleOOB(const JID& from, const OOB& oob){};

private:
  const char* m_defaultPassword;

  uint32_t m_id;
  Client* m_client;
  Registration* m_registration;
  ConnectionError m_connection_status;
  std::string m_user;
  const char* m_password;

  void loginAfterRegistration();
};

XMPP_Client::XMPP_Client(uint32_t id, const char* domain, const char* password, const char* server)
    : m_id(id),
      m_registration(nullptr),
      m_connection_status(ConnNotConnected),
      m_user(USER_PREFIX),
      m_password(DEFAULT_PASSWORD)
{
  m_user += std::to_string(m_id);
  JID jid;
  jid.setUsername(m_user);
  jid.setServer(domain);

  m_client = new Client(jid, DEFAULT_PASSWORD);

  m_client->setServer(server);
  m_client->disableRoster();
  m_client->registerConnectionListener(this);
  m_client->registerMessageHandler(this);
}

XMPP_Client::~XMPP_Client()
{
  m_registration->removeRegistrationHandler();
  delete m_registration;
  m_client->removeConnectionListener(this);
  delete m_client;
}

void XMPP_Client::onConnect()
{
  printf("Client %s connected\n", m_client->jid().bare().c_str());
}

void XMPP_Client::onDisconnect(ConnectionError e)
{
  m_connection_status = e;
  switch (m_connection_status)
  {
    case ConnAuthenticationFailed:
      printf("auth failed. reason: %d\n", m_client->authError());
      registerAccount();
      break;
    case ConnStreamError:
      printf("Stream error. Reason: %s\n", m_client->streamErrorText().c_str());
      break;
    case ConnIoError:
      printf("IO error\n");
      break;
    default:
      printf("Other error: %d\n", m_connection_status);
      break;
  }
}

void XMPP_Client::handleMessage(const Message& msg, MessageSession* session)
{
  printf("From: %s. To: %s. Msg: %s\n", msg.from().bare().c_str(), msg.to().bare().c_str(),
         msg.body().c_str());
}

void XMPP_Client::handleRegistrationFields(const JID& from, int fields, std::string instructions)
{
}

void XMPP_Client::handleRegistrationResult(const JID& from, RegistrationResult regResult)
{
  printf("RegistrationResult %d\n", regResult);
  if (regResult == RegistrationSuccess)
  {
    loginAfterRegistration();
  }
}

void XMPP_Client::loginAfterRegistration()
{
    m_client->setUsername(m_user);
    m_client->setPassword(m_password);
    m_client->login();
}

void XMPP_Client::connect()
{
  m_client->connect(false);
}

bool XMPP_Client::needRegistration() const
{
  return (m_connection_status && m_client->authError() == SaslNotAuthorized);
}

void XMPP_Client::registerAccount()
{
  printf("Registering: %s\n", m_client->username().c_str());
  m_registration = new Registration(m_client);
  m_registration->registerRegistrationHandler(this);

  RegistrationFields fields;
  fields.username = m_user;
  fields.password = m_password;

  m_client->setUsername("");
  m_client->setPassword("");
  m_client->connect(false);

  m_registration->createAccount(Registration::FieldUsername | Registration::FieldPassword, fields);
}

void XMPP_Client::loop()
{
  while (1)
  {
    m_client->recv();
  };
}

int XMPP_Client::getSocket() const
{
    return ((ConnectionTCPBase*)(m_client->connectionImpl()))->socket();
}

int main()
{
    /*std::vector<XMPP_Client*> clients;
    clients.resize(NUM_USERS);
    
    int id = 0;
    for (auto client : clients)
    {
        client = new XMPP_Client(id++, XMPP_DOMAIN, DEFAULT_PASSWORD, XMPP_SERVER);
    }*/

    int x = 800;
    XMPP_Client client1(x++, XMPP_DOMAIN, DEFAULT_PASSWORD, XMPP_SERVER);
    XMPP_Client client2(x++, XMPP_DOMAIN, DEFAULT_PASSWORD, XMPP_SERVER);
    XMPP_Client client3(x++, XMPP_DOMAIN, DEFAULT_PASSWORD, XMPP_SERVER);
    XMPP_Client client4(x++, XMPP_DOMAIN, DEFAULT_PASSWORD, XMPP_SERVER);



  client1.connect();
  client2.connect();
  client3.connect();
  client4.connect();



  //fd_set client_sockets;
  //client.sockets

  //select(0, x, 0, 0, nullptr);

  //client.loop();

  system("pause");
}
