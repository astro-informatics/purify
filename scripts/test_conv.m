clear 
close all

FID = -1;
 t = 0;
 file ='G.txt';
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
n = 2; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
ntot =25600;
Gmat=zeros(10,ntot );
for i=1:163707
Gmat(data(i,1),data(i,2)) = data(i,3)+1i*data(i,4);
end

for i=1:10
    figure,plot(abs(Gmat(i,:)))
    figure,imagesc(fftshift(abs(reshape(Gmat(i,:),[sqrt(ntot),sqrt(ntot)])))),axis image
end
%%  gaussian kernel
clear 
close all
clc
FID = -1;
 t = 0;
 file ='out1.txt';
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
n = 2; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
ntot =128*128
nx =sqrt(ntot);
outF_ =zeros(ntot,1);
outF_(data(:,1)) = data(:,3);
outF=fftshift(reshape(outF_,[nx,nx]));

%  FID = -1;
%  t = 0;
%  file ='out2.txt';
%  while FID<1 && t<100
%      t=t+1;
%     FID = fopen(file);
%  end
% form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% % n = 1; % numbers of lines representing a text 
% out = textscan(FID, form,'headerlines',n);
% fclose(FID);
% data=cell2mat(out);
% out2_ =zeros(ntot,1);
% out2_(data(:,1)) = data(:,3);
% out2=reshape(out2_,[nx,nx]);

 FID = -1;
 t = 0;
 file ='vect.txt';
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% line = '%n %n '
n = 2; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines', n);
fclose(FID);
data=cell2mat(out);
in2_ =zeros(ntot,1);
in2_(data(:,1)) = data(:,3);
in2=reshape(in2_,[nx,nx]);



 FID = -1;
 t = 0;
 file ='gbis.txt';
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% n = 1; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
in1_ =zeros(ntot,1);
in1_(data(:,1)) = data(:,3);
in1=fftshift(reshape(in1_,[nx,nx]));


mres_ = (conv2((in1),in2,'same'));
mres__= conv2(in1,in2,'same');
% 
 figure,subplot 121, imagesc(abs((in1))), axis image,colorbar; 
 subplot 122,imagesc(log10(in2)),axis image,colorbar
%  figure(2),imagesc(log10(abs(mres_)))
 figure,subplot 121, imagesc((abs((outF)))), axis image,colorbar; 
subplot 122,imagesc(log10(abs((mres_-outF)))),axis image,colorbar
% figure(4),subplot 121, imagesc(fftshift(out2)), axis image,colorbar; 
% subplot 122,imagesc(fftshift(mres__)),axis image,colorbar

%% chirp in image space
% clear 
% close all
n = 2; % numbers of lines representing a text 
ntot =64*64
nx =sqrt(ntot);
 FID = -1;
 t = 0;
 file ='chirp_fourier.txt';
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% n = 1; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
in_ =zeros(nx,nx);
in_bis = zeros(ntot,1);
for i=1:length(data(:,1))
in_bis(data(i,1),data(i,2)) = data(i,3)+1i*data(i,4);
in_(data(i,1),data(i,2)) = data(i,3)+1i*data(i,4);

end
% in_1=reshape(in_bis,[nx,nx]);
figure,imagesc(real(in_)),axis image
%% chirp kernels
clc
clear
close all
n = 2; % numbers of lines representing a text 
ntot =64*64
nx =sqrt(ntot)
for k=0:9

 FID = -1;
 t = 0;
 file =['chirp',num2str(k),'.txt'];
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% n = 1; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
in1_ =zeros(ntot,1);
in1_(data(:,1)) = data(:,3)+1i*data(:,4);
in1=reshape(in1_,[nx,nx]);
% figure,imagesc(abs(in1)),axis image
%
 FID = -1;
 t = 0;
 file =['G',num2str(k),'.txt'];
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
% line = '%n %n '
n = 2; % numbers of lines representing a text 
out = textscan(FID, form,'headerlines', n);
fclose(FID);
data=cell2mat(out);
in2_ =zeros(ntot,1);
in2_(data(:,1)) = (data(:,3)+1i*data(:,4));
in2=ifftshift(reshape(in2_,[nx,nx]));
% figure,imagesc(abs(in2)),axis image



FID = -1;
 t = 0;
 file =['GF',num2str(k),'.txt'];
 while FID<1 && t<100
     t=t+1;
    FID = fopen(file);
 end
form='%f %f %f %f '; % we have 5 columns, then use 3 %f
out = textscan(FID, form,'headerlines',n);
fclose(FID);
data=cell2mat(out);
outF_ =zeros(ntot,1);
outF_(data(:,1)) = (data(:,3))+1i*data(:,4);
outf=ifftshift(reshape(outF_,[nx,nx]));

mres_ = conv2((in2),(in1),'same');
diff = mres_ -outf;
sum(sum(diff(:)))
figure,
subplot 131,imagesc(abs(outf)),axis image,colorbar
subplot 132,imagesc(abs(mres_)),axis image,colorbar
subplot 133,imagesc(real(diff)),axis image,colorbar

end