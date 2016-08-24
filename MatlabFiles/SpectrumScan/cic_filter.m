function y=cic_filter(x,M)

len=length(x);
coff=ones(1,16)/16;

y1=filter(coff,1,x);
y1=floor(y1);
y2=filter(coff,1,y1);
y2=floor(y2);
y3=filter(coff,1,y2);
y3=floor(y3);
y4=filter(coff,1,y3);
y4=floor(y4);
y5=filter(coff,1,y4);
% y5=floor(y5);
% y6=filter(coff,1,y5);
%y5=floor(y5/16^5);

p=zeros(1,len/M);
for t=1:len/M
    p(t)=y5(t*M);
end

y=p;
