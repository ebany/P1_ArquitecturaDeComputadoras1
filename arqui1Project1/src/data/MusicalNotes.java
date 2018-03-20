/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;

/**
 *
 * @author edwin
 */
public class MusicalNotes {
    private String noteName;
    private Integer velocity;
    private Integer octave;
    private Integer key;

    public MusicalNotes(String noteName, Integer velocity, Integer octave, Integer key) {
        this.noteName = noteName;
        this.velocity = velocity;
        this.octave = octave;
        this.key = key;
    }

    public String getNoteName() {
        return noteName;
    }

    public void setNoteName(String noteName) {
        this.noteName = noteName;
    }

    public Integer getVelocity() {
        return velocity;
    }

    public void setVelocity(Integer velocity) {
        this.velocity = velocity;
    }

    public Integer getOctave() {
        return octave;
    }

    public void setOctave(Integer octave) {
        this.octave = octave;
    }

    public Integer getKey() {
        return key;
    }

    public void setKey(Integer key) {
        this.key = key;
    }
    
    
}
