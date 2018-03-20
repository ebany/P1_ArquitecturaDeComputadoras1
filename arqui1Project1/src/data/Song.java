/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package data;

import java.util.List;

/**
 *
 * @author edwin
 */
public class Song {
    private List<MusicalNotes> notes;
    private String nameSong;

    public Song(List<MusicalNotes> notes, String nameSong) {
        this.notes = notes;
        this.nameSong = nameSong;
    }

    public List<MusicalNotes> getNotes() {
        return notes;
    }

    public void setNotes(List<MusicalNotes> notes) {
        this.notes = notes;
    }

    public String getNameSong() {
        return nameSong;
    }

    public void setNameSong(String nameSong) {
        this.nameSong = nameSong;
    }
    
    
    
    
}
