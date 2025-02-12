package bgu.spl.net.srv;
import java.util.LinkedList;

public class Channel {
    private String topicName;
    private LinkedList<Integer> usersSubByConnectionId;
    private LinkedList<String> usersSubByUserName;

    public Channel (String name ){
        this.topicName = name;
        usersSubByConnectionId = new LinkedList<Integer>();
        usersSubByUserName = new LinkedList<String>();
    }

    public LinkedList<Integer> getUsersSubByConnectionID (){
        return usersSubByConnectionId;
    }

    public LinkedList<String> getUsersSubByUserName (){
        return usersSubByUserName;
    }

    public void removeUserFromChannelSub(User user, int CID){
        usersSubByConnectionId.remove((Integer)CID);
        usersSubByUserName.remove(user.getUsername());
    }

    public void AddUserToChannelSub(User user, int CID){
        usersSubByConnectionId.add(CID);
        usersSubByUserName.add(user.getUsername());
    }
}
