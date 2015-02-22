#include "gloox.h"
#include "connectionlistener.h"
#include "messagehandler.h"
#include "message.h"
#include "client.h"
#include <cstdio>

using namespace gloox;

class XMPP_Client : public ConnectionListener, MessageHandler
{
public:
	XMPP_Client();

	~XMPP_Client();
	void start();
	void receive();

	//ConnectionListener
	virtual bool onTLSConnect(const CertInfo& info) { return true; }
	virtual void onConnect();
	virtual void onDisconnect(ConnectionError e);

	//MessageHandler
	virtual void handleMessage(const Message& msg, MessageSession* session = 0);


private:
	Client* m_client;
};

void XMPP_Client::onConnect()
{
	printf("Component %s connected\n", m_client->jid().full().c_str());
}

void XMPP_Client::onDisconnect(ConnectionError e)
{
	switch (e)
	{
	case ConnAuthenticationFailed:
		printf("auth failed. reason: %d\n", component->authError());
		break;
	case ConnStreamError:
		printf("Stream error. Reason: %s\n", component->streamErrorText().c_str());
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
	printf("From: %s. To: %s. Msg: %s\n", msg.from().bare().c_str(), msg.to().bare().c_str(), msg.body().c_str());

	if (msg.from().username().find("device") != std::string::npos)
	{
		Message msgToSend(Message::Chat, msg.from(), "OkOk!");
		msgToSend.setFrom(msg.to());
		component->send(msgToSend);
	}
	else if (msg.from().resource().find("device") != std::string::npos)
	{
		JID chatFromJID(msg.to());

		JID chatToJID(msg.from());
		chatToJID.setResource("");

		Message msgToSend(Message::Groupchat, chatToJID, "Chat OkOk!");
		msgToSend.setFrom(chatFromJID);

		component->send(msgToSend);
	}
}

int main()
{
	system("pause");
}