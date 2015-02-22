#include "gloox.h"
#include "component.h"
#include "connectionlistener.h"
#include "subscriptionhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "iqhandler.h"
#include "mucinvitationhandler.h"

#include "message.h"
#include "presence.h"
#include "mucroom.h"

#include <string>

using namespace gloox;

class XMPP_Client: public ConnectionListener, SubscriptionHandler, MessageHandler, IqHandler, MUCInvitationHandler
{
public:
    XMPP_Client();

    ~XMPP_Client();
    void start();
    void receive();

    virtual bool onTLSConnect(const CertInfo& info ) {return true;}
    virtual void onConnect() 
    {
        printf( "Component %s connected\n", component->jid().full().c_str());
    }
    
    virtual void onDisconnect( ConnectionError e ) 
    {
        switch (e)
        {
        case ConnAuthenticationFailed:
            printf( "auth failed. reason: %d\n", component->authError() );
            break;
        case ConnStreamError:
            printf( "Stream error. Reason: %s\n", component->streamErrorText().c_str() );
            break;
        case ConnIoError:
            printf( "IO error\n");
            break;
        default:
            printf( "Other error: %d\n", e );
            break;
        }
    }

    virtual void handleSubscription(const Subscription& subscription)
    {
        switch (subscription.subtype())
        {
          case Subscription::Subscribe:
              printf("SUB request.");
              {         
                Subscription subscriptionResponse(Subscription::Subscribed, subscription.from());
                subscriptionResponse.setFrom(subscription.to());
                component->send(subscriptionResponse);

                Presence presence(Presence::DND, subscription.from());
                presence.setFrom(subscription.to());
                component->send(presence);
              }
              break;
          case Subscription::Subscribed:
              printf("SUB confirmation.");
              break;
          case Subscription::Unsubscribe:
              printf("UnSUB request.");
              break;
          case Subscription::Unsubscribed:
              printf("UnSUB confirmation.");
              break;
          default:
              printf("Unknown SUB");
              break;
        }
        
        printf("from: %s to: %s\n", subscription.from().bare().c_str(), subscription.to().bare().c_str());

    }

    virtual void handleMessage( const Message& msg, MessageSession* session = 0 )
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

    virtual bool handleIq(const IQ& iq)
    {
        printf("handleIq\n");
        return false;
    }
    
    virtual void handleIqID(const IQ& iq, int context )
    {
      printf("handleIqID\n");
    }

    virtual void handleMUCInvitation( const JID& room, const JID& from, const JID& to, const std::string& reason,
                            const std::string& body, const std::string& password,
                            bool cont, const std::string& thread )
    {
        printf("MUC invitation from: %s, room: %s\n", from.full().c_str(), room.full().c_str());

        JID room_join(room);
        room_join.setResource(to.username()+"_alias");

        Presence presence(Presence::Available, room_join);
        presence.setFrom(to);
        component->send(presence);
    }

private:
    Component* component; 
};

XMPP_Client::XMPP_Client()
    : component(new Component("jabber:component:accept", "192.168.1.2", "component.city.gov", "abcd", 8888)),
      MUCInvitationHandler(component)
{
    component->registerConnectionListener(this);
    component->registerSubscriptionHandler(this);
    component->registerMessageHandler(this);
    component->registerIqHandler(this, ExtDiscoInfo);
    component->registerMUCInvitationHandler(this);
    component->registerStanzaExtension( new MUCRoom::MUCUser() );
}

XMPP_Client::~XMPP_Client()
{
    delete component;
}

void XMPP_Client::start()
{
    if( !component->connect() )
    {
        printf("Connection failed...\n");
    }
}

int main()
{
  XMPP_Client* comp = new XMPP_Client();
  comp->start();

  system("pause");
  return 0;
}

