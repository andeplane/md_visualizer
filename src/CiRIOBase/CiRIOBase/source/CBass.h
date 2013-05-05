#include <bass.h>

class CBass {
 public:
  HSTREAM stream;
  double Seconds;
  double Position;
  int Cue;
  double BPM;
  int Whole;
  int Double;
  int Quad;
  int Time;

  void Initialize(string filename, double bpm) {
    BASS_Init(-1, 44100, 0, 0, NULL); // initialize default output device
    stream=BASS_StreamCreateFile(FALSE, filename.c_str(), 0, 0, 0); // create a stream (from file in command-line)
    BPM = bpm;
    Time = 0;
  }  

  void Play() {
    BASS_ChannelPlay(stream, FALSE); // start playing it
  }
  
  void GetPosition() {
    Whole = -1;
    Double = -1;
    Quad = -1;

    QWORD   qwPos = BASS_ChannelGetPosition (stream,0);
    Seconds = BASS_ChannelBytes2Seconds(stream,qwPos);
    Position = BPM*Seconds/60.0 /16.0;
    int tmp = ((Position - (int)Position)*64.0);
    //cout << newCue << endl;
    if (Quad == tmp)
      return;
    Quad = tmp;
    Time++;
    
  }

  void SetPosition(int pos) {
    BASS_ChannelSetPosition (stream,pos,0);
    
  }

  


  void Stop() {
    BASS_StreamFree(stream); // free the stream
    BASS_Free(); // free the output device
  }


};
