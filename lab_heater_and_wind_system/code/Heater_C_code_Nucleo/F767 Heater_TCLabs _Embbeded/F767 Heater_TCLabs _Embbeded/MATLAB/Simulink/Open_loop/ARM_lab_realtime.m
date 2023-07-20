function t = ARM_lab_realtime(input_)


persistent icount s

%% Create serial port 
if (isempty(icount))
 
   try 
      s=serialport('COM3',115200,"Timeout",5);
      configureTerminator(s,"CR") ;
   catch
      warning('Unable to connect, user input required')
      com_port = input('Specify port (e.g. COM5 for Windows or /dev/ttyUSB0 for Linux): ','s');
      s=serialport(com_port,115200,"Timeout",5);
      configureTerminator(s,"CR") ;
   end
icount = 0;
end   


% increment counter
icount = icount + 1;

%% Write PWM
 writeline(s,['q1 ',num2str(max(0,min(round(input_(1)),255)))]); readline(s);
 writeline(s,['q2 ',num2str(max(0,min(round(input_(2)),255)))]); readline(s);
  
  
%% Read temperature
 writeline(s,"t1");
 t(1)=str2num(readline(s)); %Read T1
 writeline(s,"t2");
 t(2)=str2num(readline(s)); %Read T2

%% Clear function in Simulink callbacks - Model Properties => celar ARM_lab_realtime 
