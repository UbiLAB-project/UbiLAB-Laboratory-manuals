%% Setup serial TCLab
clear,clc
    baud=115200 ; %baud 115200 500000 256000 950000
    s=serialport('COM2',baud,"Timeout",10);
    configureTerminator(s,"CR") 

    
%% Start receiving data 
    iter=10;
    TEMP=zeros(2,iter); %Receive buffer
    TEMP_filter=zeros(2,iter); %Receive buffer
    time=zeros(1,iter); %Receive buffer
  
    disp('OPEN PORT!')
    %START PWM
     writeline(s,"q1 0"); readline(s);
     writeline(s,"q2 0"); readline(s);
  
  %% REAL TIME PLOT  
  
    figure;
    delay=0.1;
    axis([0 iter*delay 0 75]); 
  %% START Acquiring data
            
        tic
        for i=1:iter
            writeline(s,"t1");
            TEMP(1,i)=str2num(readline(s));
            writeline(s,"t2");
            TEMP(2,i)=str2num(readline(s)); 
            time(i)=toc;
            %filter
%             TEMP_filter(1,i)=smooth( TEMP(1,1:i),10);
%             TEMP_filter(2,i)=smooth( TEMP(2,1:i),10); 
            %Real time plot
            plot(time(1:i)',[TEMP(1,1:i)' TEMP(2,1:i)'],'linewidth',2);
            
            axis([0 iter*delay 0 75]); 
            pause(delay);
        end

        
%% Close port
    clear s
    toc
    disp('CLOSED PORT!')

%% Save to TXT file 
    %  Data=[time' TEMP(1,:)' TEMP(2,:)'];
    %  writematrix(Data,'myData.txt','Delimiter',';');
    % 
    %  %Load data 
    %   dataTXT=load('myData.txt');

    %T = table(Data)

    

%% Plot data
 TEMP_filter(1,1:end)=smooth( TEMP(1,1:end),10);
 TEMP_filter(2,1:end)=smooth( TEMP(2,1:end),10);

figure(2)
    subplot(2,1,1)
        plot(time(1:end)', [TEMP(1,1:end)' TEMP_filter(1,1:end)'],'linewidth',2)
        title('TEMP 1')
        xlabel('time');ylabel('T[C]'),legend('RAW','Filtered')
    subplot(2,1,2)
        plot(time(1:end),  [TEMP(2,1:end)' TEMP_filter(2,1:end)'],'linewidth',2)
        title('TEMP 2')
        xlabel('time');ylabel('T[C]'),legend('RAW','Filtered')


        
        
 %% Cast to numbers
%     for i=1:iter
%         time(i)=typecast(uint8(RX_buffer(i,1:4)),  'uint32'); %Timer SysTick
%         PWM_val(i)=typecast(uint8(RX_buffer(i,5:8)),  'uint32');   %Timer SysTick
%         current_ref(i)=typecast(uint8(RX_buffer(i,9:12)),  'single'); %float
%         current(i)=typecast(uint8(RX_buffer(i,13:16)), 'single'); %float
%         RPM_ref(i)=typecast(uint8(RX_buffer(i,17:20)), 'single'); %float
%         RPM(i)=typecast(uint8(RX_buffer(i,21:24)), 'single'); %float
%         angle_ref(i)=typecast(uint8(RX_buffer(i,25:28)), 'single'); %float
%         angle(i)=typecast(uint8(RX_buffer(i,29:bytes_rec)), 'single'); %float
%         
%     end

        
