%% Setup serial TCLab
    baud=115200 ; %baud 115200 500000 256000 950000
    s=serialport('COM2',baud,"Timeout",10);
    configureTerminator(s,"CR") ;