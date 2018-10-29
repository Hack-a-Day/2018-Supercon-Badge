///////////////////////////////////////////////////////////////////////////////
// Badge Nyancat demo: an adaptation of the (in)famous internet meme
//
// This is an animation that plays at full screen while playing a simplified
// rendition of the video soundtrack.
//
// Pressing '0' on the keyboard mutes the music while animation continues.
// Pressing '9' on the keyboard restarts music if muted, otherwise no effect.

#include "nyancat.h"

#ifdef NYANCAT_DEMO

///////////////////////////////////////////////////////////////////////////////
// Utility function to read music measure array as specified by parameters
// and play the associated notes in that measure.

uint32_t play_next_note(uint8_t measure_index, uint8_t note_index, uint8_t *next_note)
{
    uint32_t duration = 0; // Time (in milliseconds) to hold these notes
    uint8_t  nyancat_index = 0; // Used to index into nyancat_measures
    uint8_t  voice_index = 0;   // Loop through each sound_set_note

    *next_note = 0; // Will stay at zero if we've reached the end of this measure

    nyancat_index = note_index * 3;
    if (nyancat_index < nyancat_measure_limits[measure_index])
    {
        for (voice_index = 0; voice_index < 2; voice_index++)
        {
            sound_set_note(nyancat_measures[measure_index][nyancat_index+voice_index],voice_index);
        }

        duration = nyancat_durations[nyancat_measures[measure_index][nyancat_index+2]];
        *next_note = note_index+1;
    }

    return duration;
}

///////////////////////////////////////////////////////////////////////////////
// Entry point to start nyancat demo

void nyancat(void)
{
    // Current screen coordinates for drawing
    uint16_t x = 0;
    uint8_t y = 0;

    // Single line 320 pixels wide buffer, each 32-bit value in 0xNNRRGGBB format
    // NN = Not used, RR = 8-bit red, GG = 8-bit green, BB = 8-bit blue
    uint32_t line[320];

    uint16_t i = 0; // General loop counter
    uint8_t  frame_idx = 0; // Which frame are we currently displaying?
    uint16_t frame_data = 0;// Which byte in frame data are we on?
    uint8_t  low_nibble = 0;// When 1, the next data comes from low nibble of frame_data
    uint32_t draw_color = 0;// Currently used RGB color
    uint8_t  run_length = 0;// Number of pixels to draw using current color

    // Image scaling control. 1=full scale, 2=half scale, 4=quarter, etc.
    const uint8_t multiplier = 4; // Only quarter-resolution version is on badge by default.
    uint8_t multiply_loop=0;

    // Animation speed control
    const uint8_t milliseconds_per_frame = 70;
    uint32_t time_for_next_frame = millis();

    // Audio playback control
    // audio_state is the state machine that sequences measures into a song.
    // Each measure has zero or more 'notes' (different from sheet music notes)
    // Each 'note' is a set of three frequencies to be held for a specific time.
    uint32_t time_for_audio_update = millis();
    uint8_t audio_state = 0;
    uint8_t measure_index = 0;
    uint8_t note_index = 0;
    uint8_t next_note = 0;
    uint8_t mute = 0;

    // See if user wants to mute or, if muted, wants to restart.
    uint8_t get_stat, char_out;

    enable_display_scanning(0); //Shut off auto-scanning of character buffer
    sound_set_note(0,2); // We're only using 2 out of 3 voices
    
    while(1) //Loop forever
    {
        // Is it time for the next frame?
        if (millis() >= time_for_next_frame)
        {
            // Set time threshold to trigger next frame render
            time_for_next_frame += milliseconds_per_frame;

            // Reset per-frame variables
            frame_data=0;
            low_nibble=0;

            // If we're past the end, start from the beginning
            if(frame_idx > 11)
            {
                frame_idx = 0;
            }

            // Decode and render image line-by-line
            for (y=0;y<240;y+=multiplier)
            {
                x = 0;

                while (x < 320)
                {
                    // Unpack run-length encoded values. See nyancat.h for
                    // details on how bytes are packed in frame data.
                    if (low_nibble)
                    {
                        draw_color = cat4_palette[cat4_frames[frame_idx][frame_data]&0xF];
                        run_length = cat4_frames[frame_idx][frame_data+1];
                        frame_data += 2;
                        low_nibble = 0;
                    }
                    else
                    {
                        draw_color = cat4_palette[cat4_frames[frame_idx][frame_data] >> 4];
                        run_length = ((cat4_frames[frame_idx][frame_data] & 0xF)<<4) |
                                     (cat4_frames[frame_idx][frame_data+1]>>4);
                        frame_data++;
                        low_nibble = 1;
                    }

                    // Draw decoded run into line buffer
                    for (i = 0; i < run_length; i++)
                    {
                        for (multiply_loop=0; multiply_loop<multiplier; multiply_loop++)
                        {
                            line[x+i*multiplier+multiply_loop] = draw_color;
                        }
                    }

                    // Advance X coordinate on this line
                    x += run_length*multiplier;
                }

                // Transfer the decoded line to screen
                tft_set_write_area(0,y,319,multiplier);
                TFT_24_7789_Write_Command(0x2C);
                for (multiply_loop=0; multiply_loop<multiplier; multiply_loop++)
                {
                    for (i=0;i<320;i++)
                    {
                        draw_color = line[i];
                        TFT_24_7789_Write_Data3((draw_color>>16)&0xFF,(draw_color>>8)&0xFF,(draw_color>>0)&0xFF);
                    }
                }
            }

            // Advance frame index
            frame_idx++;
        }

        // Is it time for the next audio update?
        if (millis() >= time_for_audio_update)
        {
            if (audio_state != 0xFF)
            {
                // Retrieve current measure from sequence of measures
                measure_index = nyancat_sequence[audio_state];

                // Play the next note in that measure
                time_for_audio_update += play_next_note(measure_index,note_index,&next_note);
                if (next_note==0)
                {
                    // This measure has come to an end, advance to next.
                    audio_state++;
                    if (audio_state >= NYANCAT_SEQUENCES)
                    {
                        // If there are no more measures, restart from beginning
                        audio_state = 0;
                    }
                }

                // Remember which note in a measure we'll be playing next
                note_index = next_note;
            }
            else
            {
                // Fallback - silence for 10 seconds.
                sound_set_note(0,0);
                sound_set_note(0,1);
                sound_set_note(0,2);
                time_for_audio_update += 10000;
            }
        }

        // Check for specific keys that change behavior.
        get_stat = stdio_get(&char_out);
        if (get_stat!=0)
        {
            // '0' immediately mutes music.
            if (char_out == '0')
            {
                mute = 1;
                sound_set_note(0,0);
                sound_set_note(0,1);
                sound_set_note(0,2);
                audio_state = 0xFF;
            }
            // '9' will restart music if muted, otherwise no effect.
            else if (char_out == '9' && mute)
            {
                mute = 0;

                time_for_audio_update = millis();
                audio_state = 0;
                note_index = 0;
            }
        }
    }
    //Badge will need to be reset to continue    
}

#endif // NYANCAT_DEMO