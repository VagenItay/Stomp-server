package bgu.spl.net.impl;

import java.util.concurrent.ConcurrentHashMap;
import bgu.spl.net.srv.Channel;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.User;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentHashMap<Integer, ConnectionHandler<T>> mapIdToHandler = new ConcurrentHashMap<>();
    private ConcurrentHashMap<Integer, User> CIDToUsers_ = new ConcurrentHashMap<>();
    private ConcurrentHashMap<String, User> AllUsers_ = new ConcurrentHashMap<>();
    private ConcurrentHashMap<String, Channel> topicToChannel_ = new ConcurrentHashMap<>();
    private int messageIdCounter;

    public ConnectionsImpl(ConcurrentHashMap<String, User> AllUsersInServer) {
        this.AllUsers_ = AllUsersInServer;
        this.messageIdCounter = 1;
    }

    @Override
    public boolean send(int connectionId, T msg) { // boolean if error occured while send
        boolean ans = false;
        ConnectionHandler<T> ch = (ConnectionHandler<T>) mapIdToHandler.get(connectionId);
        if(ch!=null){
            ch.send(msg);
            ans = true;
        } else {
            ans = false;
        }
        return ans;
    }

    @Override
    public void send(String channel, T msg) {
        Channel curr = topicToChannel_.get(channel);
        for (Integer connectionIdUser : curr.getUsersSubByConnectionID()) {
            send(connectionIdUser, msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        CIDToUsers_.remove(connectionId);
        mapIdToHandler.remove(connectionId);
    }

    @Override
    public ConcurrentHashMap<Integer, ConnectionHandler<T>> getMapIdToHandler() {
        return mapIdToHandler;
    }

    @Override
    public ConcurrentHashMap<Integer, User> getCIdtoUser() {
        return CIDToUsers_;
    }

    @Override
    public ConcurrentHashMap<String, User> getAllUsers() {
        return AllUsers_;
    }

    @Override
    public ConcurrentHashMap<String, Channel> getTopicToChannel() {
        return topicToChannel_;
    }

    @Override
    public int getMessageIdCounter() {
        return messageIdCounter;
    }

    @Override
    public void IncreaseMessageIdCounter() {
        this.messageIdCounter++;
    }

    @Override
    public void addConnectionHandlerToMap(ConnectionHandler<T> ch, int id){
        this.mapIdToHandler.put(id, ch);
    }
}