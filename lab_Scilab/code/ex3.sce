//Amplitude Shift Keying, Frequency Shift Keying And Phase Shift keying waveform generation
clc;
clear;
xdel(winsid());
sym=10;//no. of symbols
g=[1 1 1 0 1 0 0 1 0 1 ]//binary data
f1=1;f2=2;//frequencies of carrier
t=0:2*%pi/99:2*%pi;//range of time
//ASK
cp=[];bit=[];mod_ask=[];mod_fsk=[];mod_psk=[];cp1=[];cp2=[];
for n=1:length(g);//ASK modulation  // Zeros and ones are inserted for proper plot of message signal
    if g(n)==0;
        die=zeros(1,100);   
    else g(n)==1;
        die=ones(1,100);
    end
    c_ask=sin(f1*t);
    cp=[cp die];
    mod_ask=[mod_ask c_ask];
    end 
ask=cp.*mod_ask;//ASK modulated signal

//FSK
for n=1:length(g);
    if g(n)==0;
        die=ones(1,100); 
        c_fsk=sin(f1*t); 
    else g(n)==1;
        die=ones(1,100);
        c_fsk=sin(f2*t);
    end
    cp1=[cp1 die];
    mod_fsk=[mod_fsk c_fsk];
end
fsk=cp1.*mod_fsk;//FSK molated signal

//PSK
for n=1:length(g);
    if g(n)==0;
        die=ones(1,100); 
        c_psk=sin(f1*t); 
    else g(n)==1;
        die=ones(1,100);
        c_psk=-sin(f1*t);
    end
    cp2=[cp2 die];
    mod_psk=[mod_psk c_psk];
end
psk=cp2.*mod_psk;//PSK modulated signal
subplot(4,1,1);plot(cp,'LineWidth',1.5);//plot binary signal
xgrid;
title('Binary Signal');//title
mtlb_axis([0 100*length(g) -2.5 2.5]); //axis range 
subplot(4,1,2);plot(ask,'LineWidth',1.5);//plot of ASK modulated signal
xgrid;
title('ASK modulation');//title of plot
mtlb_axis([0 100*length(g) -2.5 2.5]);//axis range
subplot(4,1,3);plot(fsk,'LineWidth',1.5);//plot of FSK modulated signal
xgrid;
title('FSK modulation');//title of plot
mtlb_axis([0 100*length(g) -2.5 2.5]);//axis range
subplot(4,1,4);plot(psk,'LineWidth',1.5);//plot of PSK modulated signal
xgrid;
title('PSK modulation');//title of plot
mtlb_axis([0 100*length(g) -2.5 2.5]);//range of axis
//Result: This experiment results plots of binary data, ASK modulation,FSK modulation and PSK modulation


