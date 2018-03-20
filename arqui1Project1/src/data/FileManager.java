/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;

import java.awt.Component;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Sequencer;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;


import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Track;
import views.Home;


/**
 *
 * @author edwin
 */
public class FileManager {
    
    private Component parent;
    public static final int NOTE_ON = 0x90;
    public static final int NOTE_OFF = 0x80;
    public static final String[] NOTE_NAMES = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    public FileManager(Component parent) {
        this.parent = parent;
    }

    public Component getParent() {
        return parent;
    }

    public void setParent(Component parent) {
        this.parent = parent;
    }   
    
    public Boolean openFile(){
        FileNameExtensionFilter filter = new FileNameExtensionFilter("MID","mid");
        JFileChooser filechooser = new JFileChooser();
        filechooser.setFileFilter(filter);     
        int opc = filechooser.showOpenDialog(parent);
        if (opc == JFileChooser.APPROVE_OPTION){            
            String pathFileMidi = filechooser.getSelectedFile().getPath();
            try {
                //this.processMidi(pathFileMidi);
                List<MusicalNotes> notes = this.midiTest(pathFileMidi);
                String fileName = filechooser.getSelectedFile().getName();                
                Home.songs.add(new Song(notes,fileName));
                return true;
            } catch (Exception ex) {
                Logger.getLogger(FileManager.class.getName()).log(Level.SEVERE, null, ex);
            }
                
        }
        return false;
    }
    
    public void processMidi(String path){
        File fileMidi = new File(path);
        if(!fileMidi.exists() || fileMidi.isDirectory() || !fileMidi.canRead()) {
            JOptionPane.showMessageDialog(parent, "Error al procesar el archivo");
        }else {
            try{
                Sequencer sequencer = MidiSystem.getSequencer();
                sequencer.setSequence(MidiSystem.getSequence(fileMidi));
                sequencer.open();
                sequencer.start();
                
                while(sequencer.isRunning()){
                    try {
                        Thread.sleep(1000); // Check every second
                    } catch(InterruptedException ignore) {
                        break;
                    }
                }               
                sequencer.stop();
                sequencer.close();                
            }catch(MidiUnavailableException mue) {
                System.out.println("Midi device unavailable!");
            } catch(InvalidMidiDataException imde) {
                System.out.println("Invalid Midi data!");
            } catch(IOException ioe) {
                System.out.println("I/O Error!");
            }
        }
    }
        
    public List<MusicalNotes> midiTest(String path) throws Exception {
        Sequence sequence = MidiSystem.getSequence(new File(path));
        List<MusicalNotes> notes = new ArrayList<>();
        int trackNumber = 0;
        for (Track track :  sequence.getTracks()) {
            trackNumber++;
            for (int i=0; i < track.size(); i++) { 
                MidiEvent event = track.get(i);
                MidiMessage message = event.getMessage();
                if (message instanceof ShortMessage) {
                    ShortMessage sm = (ShortMessage) message;
                    System.out.print("Channel: " + sm.getChannel() + " ");
                    if (sm.getCommand() == NOTE_ON) {
                        int key = sm.getData1();
                        int octave = (key / 12)-1;
                        int note = key % 12;
                        String noteName = NOTE_NAMES[note];
                        int velocity = sm.getData2();
                        if(velocity==0)
                            velocity++;
                        System.out.println("Note on, " + noteName + octave + " key=" + key + " velocity: " + velocity);
                        notes.add(new MusicalNotes(noteName,velocity,octave,key) );
                    } else if (sm.getCommand() == NOTE_OFF) {
                        int key = sm.getData1();
                        int octave = (key / 12)-1;
                        int note = key % 12;
                        String noteName = NOTE_NAMES[note];
                        int velocity = sm.getData2();
                        if(velocity==0)
                            velocity++;
                        System.out.println("Note off, " + noteName + octave + " key=" + key + " velocity: " + velocity);
                        notes.add(new MusicalNotes(noteName,velocity,octave,key) );
                    } else {
                        //System.out.println("Command:" + sm.getCommand());
                    }
                } else {
                    //System.out.println("Other message: " + message.getClass());
                }
            }
            //System.out.println();
        }
        return notes;
    }
    
}
