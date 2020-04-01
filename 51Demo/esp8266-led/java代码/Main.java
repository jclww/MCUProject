import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Main {

    private static String host = "192.168.9.121";
    private static Integer port = 10087;


    /**
     * 0x00 代表关灯
     * 0x01 代表开灯
     *
     * @return
     */
    public static byte[] getData() {
        return new byte[]{0x01};
    }

    public static void main(String[] args) {
        byte[] data = getData();
        byte[] receiveData = new byte[2];

        try {
            Socket client = new Socket(host, port);
            OutputStream out = client.getOutputStream();
            out.write(data);

            InputStream inputStream = client.getInputStream();
            int count = inputStream.read(receiveData);
            System.out.println(new String(receiveData));
            out.flush();
            out.close();
            client.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
