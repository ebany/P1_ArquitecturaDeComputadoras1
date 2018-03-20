/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;

import com.panamahitek.ArduinoException;
import com.panamahitek.PanamaHitek_Arduino;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import views.Home;

/**
 *
 * @author edwin
 */
public class ManagerGame implements Runnable{
    public Thread t;      
    public PanamaHitek_Arduino arduino;
    public Integer dificultad;  
    public Song song;
    
    /*public SerialPortEventListener listener = new SerialPortEventListener(){
        @Override
        public void serialEvent(SerialPortEvent spe) {
            try {
                if (arduino.isMessageAvailable()){
                    //obtener mensaje
                    String data = arduino.printMessage();
                    String[] values = data.split("$");
                    if (values.length > 1){
                        String name = values[0];
                        Integer points;
                        try{
                           points = Integer.parseInt(values[1]);
                        }catch(NumberFormatException e){
                            points = 0;
                        }
                        Home.results.add(new Result(name,points));
                    }
                }
            } catch (SerialPortException ex) {
                Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
            } catch (ArduinoException ex) {
                Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
  
    };*/
    
    public ManagerGame(Integer dificultad,Song song,PanamaHitek_Arduino arduino) {
        t = new Thread(this,"manager_game");        
        //arduino = new PanamaHitek_Arduino();
        this.arduino = arduino;
        this.song = song;
        this.dificultad = dificultad;
        /*try {       
            arduino.arduinoRXTX("COM1", 9600, listener);
        } catch (ArduinoException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        }*/
    }
    
    public void init(){
        this.t.start();
    }
    
    public void stop(){
        this.t.stop();
    }
    
    @Override
    public void run(){        
        sendRequest("Y" + dificultad + song.getNameSong()+'\n',1);
        try {
            Thread.sleep(1500);
        } catch (InterruptedException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        }
        for (MusicalNotes tmp : song.getNotes()){
            sendRequest(tmp.getVelocity()+ "" + tmp.getNoteName() + tmp.getOctave()+'\n',0);
        }
        try {
            Thread.sleep(1500);
        } catch (InterruptedException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        }
        sendRequest("H"+'\n',1);
        
    }
    
    private void sendRequest(String texto,Integer duracion){
        System.out.println("texto = " + texto);        
        try {
            this.arduino.sendData(texto);           
        } catch (ArduinoException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        } catch (SerialPortException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        }        
        this.sleepTheread(duracion);
    }
    
    private void sleepTheread(Integer duracion) {
        if(duracion>0){
            try {
                Thread.sleep(250);//250
            } catch (InterruptedException ex) {
                Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
            }
        }else
            try {
                Thread.sleep(200);//200
                
                /*try {
                switch(dificultad){
                case 1:
                //FACIL
                Thread.sleep(7000);
                break;
                case 2:
                //MEDIO
                Thread.sleep(5000);
                break;
                case 3:
                //DIFICIL
                Thread.sleep(3000);
                break;
                default:
                break;
                }                    
                } catch (InterruptedException ex) {
                Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
                Thread.interrupted();
                }*/
        } catch (InterruptedException ex) {
            Logger.getLogger(ManagerGame.class.getName()).log(Level.SEVERE, null, ex);
        }        
    }
    
}
