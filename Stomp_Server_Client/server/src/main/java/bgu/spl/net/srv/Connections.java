package bgu.spl.net.srv;

import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    ConcurrentHashMap<Integer, ConnectionHandler<T>> getMapIdToHandler();
      
    public ConcurrentHashMap<Integer, User> getCIdtoUser();
       
    public ConcurrentHashMap<String, User> getAllUsers();

    public ConcurrentHashMap<String, Channel> getTopicToChannel();

    public int getMessageIdCounter();

    public void IncreaseMessageIdCounter();

    public void addConnectionHandlerToMap(ConnectionHandler<T> ch, int id);
}
