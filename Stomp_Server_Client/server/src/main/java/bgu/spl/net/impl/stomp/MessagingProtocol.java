package bgu.spl.net.impl.stomp;

import java.util.Collection;
import java.util.Iterator;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.ConnectionsImpl;
import bgu.spl.net.srv.Channel;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

public class MessagingProtocol<T> implements StompMessagingProtocol<T> {

    private Integer connectionId_;
    private Connections<T> connections_;
    private volatile boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<T> connections) {
        connectionId_ = connectionId;
        connections_ = connections;

    }

    @Override
    public void process(T message) {

        String msg = (String) message;
        String[] splitFrame = ((String) message).split("\\R");
        if (!splitFrame[0].equals("SEND") & !splitFrame[0].equals("splitHere\n")){
            ConcurrentHashMap<String, String> LineToData = new ConcurrentHashMap<String, String>();
            for (int i = 1; i < splitFrame.length; i++) {
                String[] Line = splitFrame[i].split(":");
                LineToData.put(Line[0], Line[1]);
            }
            if (splitFrame[0].equals("CONNECT")) {
                loginCommand(LineToData, msg);
            } else if (splitFrame[0].equals("SUBSCRIBE")) {
                joinCommand(LineToData);
            } else if (splitFrame[0].equals("UNSUBSCRIBE")) {
                exitCommand(LineToData);
            } else if (splitFrame[0].equals("DISCONNECT")) {
                logoutCommand(LineToData);
            }
        } else {
            reportCommand(msg);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return this.shouldTerminate;
    }

    private void setShouldTerminate(boolean terminate) {
        this.shouldTerminate = terminate;
    }

    private String generateMessageFrame(String sendFrame, String channelTopicName_) {
        User user = connections_.getCIdtoUser().get(connectionId_);
        String[] splitBodyOfMsg = sendFrame.split("\n\n");
        String st = "MESSAGE\n";
        st += "subscription:" + user.getTopicNameToSubId().get(channelTopicName_) + "\n";
        st += "message-id:" + connections_.getMessageIdCounter() + "\n";
        connections_.IncreaseMessageIdCounter();
        st += "destination:/" + channelTopicName_ + "\n" + "\n";
        st += splitBodyOfMsg[1];// body of msg
        return st;
    }

    private String generateConnectedFrame() {
        String st = "CONNECTED\n";
        st += "version:1.2\n";
        st += "\n";
        return st;
    }

    private String generateErrorFrame(String messageHeader, String descriptionofError,
            String frameCausedError) {
        String st = "ERROR\n";
        st += messageHeader + "\n";
        st += "\n";
        st += "The message:\n";
        st += "-----\n";
        st += frameCausedError + "\n";
        st += "-----\n";
        st += descriptionofError;
        return st;
    }

    private String generateReceiptFrame(int receiptId) {
        String st = "RECEIPT\n";
        st += "receipt-id:" + receiptId + "\n";
        st += "\n";
        return st;
    }

    private void loginCommand(ConcurrentHashMap<String, String> LineToData, String msg) {

        // Client connected
        if (connections_.getCIdtoUser().containsKey(connectionId_)) {
            connections_.send(connectionId_, (T) generateErrorFrame("The client is already logged in",
                    "log out before trying again", msg)); // Client already logged in
            disconnectClient(connections_.getCIdtoUser().get(connectionId_));
            this.setShouldTerminate(true);
        } else {
            String userName = LineToData.get("login");
            String password = LineToData.get("passcode");
            if (connections_.getAllUsers().containsKey(userName)) { // User exists in server
                User u = connections_.getAllUsers().get(userName);
                if (connections_.getCIdtoUser().containsValue(u)) { // User is already logged in
                    connections_.send(connectionId_, (T) generateErrorFrame("User already logged in", "", msg));
                    connections_.disconnect(connectionId_);
                    this.setShouldTerminate(true);
                } else {
                    if (u.getPassword().equals(password)) { // password Match
                        connections_.getCIdtoUser().put(connectionId_, u);
                        connections_.send(connectionId_, (T) generateConnectedFrame()); // CONNECTED FRAME
                    } else {
                        connections_.send(connectionId_, (T) generateErrorFrame("Wrong password", "", msg)); // Wrong
                                                                                                             // password
                        connections_.disconnect(connectionId_);
                        this.setShouldTerminate(true);
                    }
                }
            } else { // New user
                User newUser = new User(userName, password);
                connections_.getAllUsers().put(userName, newUser);
                connections_.getCIdtoUser().put(connectionId_, newUser);
                connections_.send(connectionId_, (T) generateConnectedFrame());
            }
        }
    }

    private void joinCommand(ConcurrentHashMap<String, String> LineToData) {
        User user = connections_.getCIdtoUser().get(connectionId_);
        String topic = LineToData.get("destination").substring(1);
        int SubsId = Integer.valueOf(LineToData.get("id"));
        if (!connections_.getTopicToChannel().containsKey(topic)) {
            connections_.getTopicToChannel().put(topic, new Channel(topic));
        }
        user.AddSubscription(topic, SubsId);
        connections_.getTopicToChannel().get(topic).AddUserToChannelSub(user, connectionId_);
        int receiptId = Integer.valueOf(LineToData.get("receipt"));
        connections_.send(connectionId_, (T) generateReceiptFrame(receiptId));
    }

    private void exitCommand(ConcurrentHashMap<String, String> LineToData) {

        User user = connections_.getCIdtoUser().get(connectionId_);
        int SubsId = Integer.valueOf(LineToData.get("id"));
        String topic = user.getSubIdToTopic().get(SubsId);
        if (topic.equals(null))
            return;
        int receiptId = Integer.valueOf(LineToData.get("receipt"));
        user.removeSubscription(topic, SubsId);
        connections_.getTopicToChannel().get(topic).removeUserFromChannelSub(user, connectionId_);
        connections_.send(connectionId_, (T) generateReceiptFrame(receiptId));
    }

    private void reportCommand(String msg) {

        String[] splitSendFrames = msg.split("splitHere\n");

        String channelTopicName = splitSendFrames[0].split("\\R")[1].split(":/")[1];
        if (connections_.getCIdtoUser().get(connectionId_).getTopicNameToSubId().get(channelTopicName) == null) {
            connections_.send(connectionId_, (T) generateErrorFrame("User is not subscribed to channel", "",
                    "User cannot send messages to channel\nwhich he is not subscribed to"));
            this.setShouldTerminate(true);
            return;
        }
        for (int i = 0; i < splitSendFrames.length; i++) {
            connections_.send(channelTopicName, (T) generateMessageFrame(splitSendFrames[i], channelTopicName));
        }

    }

    private void logoutCommand(ConcurrentHashMap<String, String> LineToData) {
        User user = connections_.getCIdtoUser().get(connectionId_);
        int receiptId = Integer.valueOf(LineToData.get("receipt"));
        connections_.send(connectionId_, (T) generateReceiptFrame(receiptId));
        disconnectClient(user);
        setShouldTerminate(true);
    }

    private void disconnectClient(User user) {
        Collection<String> userTopics = user.getSubIdToTopic().values();
        Iterator<String> userTopicsItr = userTopics.iterator();
        while (userTopicsItr.hasNext()) {
            String topic = userTopicsItr.next();
            connections_.getTopicToChannel().get(topic).removeUserFromChannelSub(user, connectionId_);
        }
        user.removeAllSubs();
        connections_.disconnect(connectionId_);
    }
}
