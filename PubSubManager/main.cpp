#include "gloox.h"
#include "connectionlistener.h"
#include "messagehandler.h"
#include "message.h"
#include "client.h"
#include <cstdio>

using namespace gloox;

static const char* USER_PREFIX = "deviceA";
static const char* XMPP_SERVER = "xmppeval.cloudapp.net";
static const int NUM_USERS = 1000;

class XMPP_Client : public ConnectionListener, MessageHandler
{
public:
  XMPP_Client(uint32_t id);
  ~XMPP_Client();

  void tryConnect();

  // ConnectionListener
  virtual bool onTLSConnect(const CertInfo& info) { return true; }
  virtual void onConnect();
  virtual void onDisconnect(ConnectionError e);

  // MessageHandler
  virtual void handleMessage(const Message& msg, MessageSession* session = 0);

private:
  uint32_t m_id;
  Client* m_client;
};

XMPP_Client::XMPP_Client(uint32_t id) : m_id(id)
{
}

void XMPP_Client::onConnect()
{
  printf("Component %s connected\n", m_client->jid().full().c_str());
}

void XMPP_Client::onDisconnect(ConnectionError e)
{
  switch (e)
  {
    case ConnAuthenticationFailed:
      printf("auth failed. reason: %d\n", m_client->authError());
      break;
    case ConnStreamError:
      printf("Stream error. Reason: %s\n", m_client->streamErrorText().c_str());
      break;
    case ConnIoError:
      printf("IO error\n");
      break;
    default:
      printf("Other error: %d\n", e);
      break;
  }
}

void XMPP_Client::handleMessage(const Message& msg, MessageSession* session = 0)
{
  printf("From: %s. To: %s. Msg: %s\n", msg.from().bare().c_str(), msg.to().bare().c_str(),
         msg.body().c_str());
}

void XMPP_Client::tryConnect()
{
  char user[20];
  sprintf(user, "%s_%d@%s", USER_PREFIX, m_id, XMPP_SERVER);
  static const int NUM_USERS = 1000;)
	JID jid;
  jid.setUsername(user);
  jid.setServer();
  Client* client = new Client(jid, "password");
  client->registerConnectionListener(this);
  client->registerPresenceHandler(this);
  client->connect();
}

int main()
{
  system("pause");
}