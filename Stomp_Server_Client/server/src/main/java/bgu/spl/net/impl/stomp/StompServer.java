package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {

        int portNum = Integer.parseInt(args[0]);
        String server = args[1];
        if (server.equals("tpc")) {
            Server.threadPerClient(
                    portNum, // port
                    MessagingProtocol::new, // protocol factory
                    MsgEncoderDecoder::new // message encoder decoder factory
            ).serve();
        } else if (server.equals("reactor")) {

            Server.reactor(
            Runtime.getRuntime().availableProcessors(),
            portNum, // port
            MessagingProtocol::new, //protocol factory
            ()-> new MsgEncoderDecoder() //message encoder decoder factory
            ).serve();
        }
    }
}
