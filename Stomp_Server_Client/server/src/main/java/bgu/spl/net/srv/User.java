package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    private String Username;
    private String Password;
    private ConcurrentHashMap<String, Integer> topicNameToSubId;
    private ConcurrentHashMap<Integer, String> SubIdToTopic;
    private int SubsCounter;

    public User(String Username, String Password) {
        this.Username = Username;
        this.Password = Password;
        this.topicNameToSubId = new ConcurrentHashMap<String, Integer>();
        this.SubIdToTopic = new ConcurrentHashMap<Integer,String>();
    }

    public ConcurrentHashMap<String, Integer> getTopicNameToSubId() {
        return topicNameToSubId;
    }

    public ConcurrentHashMap<Integer, String> getSubIdToTopic() {
        return SubIdToTopic;
    }

    public void AddSubscription(String Topic, Integer SubscriptionId) {
        topicNameToSubId.put(Topic, SubscriptionId);
        SubIdToTopic.put(SubscriptionId, Topic);
    }

    public void removeSubscription(String Topic, Integer SubscriptionId) {
        topicNameToSubId.remove(Topic);
        SubIdToTopic.remove(SubscriptionId);
    }

    public void removeAllSubs (){
        topicNameToSubId.clear();
        SubIdToTopic.clear();
    }

    public String getUsername() {
        return this.Username;
    }

    public int getSubsCounter() {
        return this.SubsCounter;
    }

    public String getPassword() {
        return this.Password;
    }

    public void setPassword(String Password) {
        this.Password = Password;
    }
}
