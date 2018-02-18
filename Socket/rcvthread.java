import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;

public class rcvthread implements Runnable {

	private String command = "sudo ./Servo";
    private static final int sizeBuf = 50;
    private Socket 			clientSocket;
    private SocketAddress 	clientAddress;
    private int rcvBufSize;
    private byte[] rcvBuf = new byte[sizeBuf];
    
    public rcvthread(Socket clientSocket, SocketAddress clientAddress) {
        this.clientSocket  = clientSocket;
        this.clientAddress = clientAddress;
    }

	public static void shellCmd(String command) throws Exception{
		Runtime runtime = Runtime.getRuntime();
		Process process = runtime.exec(command);
		InputStream is = process.getInputStream();
		InputStreamReader isr = new InputStreamReader(is);
		BufferedReader br = new BufferedReader(isr);
		String line;
		while((line = br.readLine())!=null){
			System.out.println(line);
		}
	}

    public void run() {
	    try {
	        InputStream ins   = clientSocket.getInputStream();
	        OutputStream outs = clientSocket.getOutputStream();

	        while ((rcvBufSize = ins.read(rcvBuf)) != -1) {
	        	String rcvData = new String(rcvBuf, 0, rcvBufSize, "UTF-8");
	        	
	        	if (rcvData.compareTo("Up") == 0){

	            	System.out.println("Go!");
					try{
						shellCmd(command);
					}catch(Exception e){

					}
	            	
				}
	           	System.out.println("Received data : " + rcvData + " (" + clientAddress + ")");
	            outs.write(rcvBuf, 0, rcvBufSize);
	        }
	        System.out.println(clientSocket.getRemoteSocketAddress() + " Closed");

	    } catch (IOException e) {
	    	System.out.println("Exception: " + e);
	    } finally {
	        try {
	        	clientSocket.close();
	            System.out.println("Disconnected! Client IP : " + clientAddress);
	        } catch (IOException e) {
	        	System.out.println("Exception: " + e);
	        }
	    }
    }
}
