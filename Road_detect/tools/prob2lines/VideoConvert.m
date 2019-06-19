%read video and tranform into png then output
clc
clear
%read .mp4 or .avi and output
videoPath = 'E:\BaiduNetdiskDownload\d.avi';
videoObj = VideoReader(videoPath);

frameNumber = videoObj.NumberofFrames;
% frameNumber = 2350;
startframe = 1;

if ~exist('image5','dir')
    mkdir('image5');
    disp('create image dir success.');
end

% for i = 181:2:frameNumber
%     k = (i-181)/2+1
%     image_name=strcat('./image2/im_',num2str(k),'.jpg');   
%     frame=read(videoObj,i);           
%     imwrite(frame,image_name,'jpg');
% end
% 
for i = startframe:5:frameNumber
    n = (i+4)/5;
    disp([num2str(i/frameNumber*100),'%'])
    image_name=strcat('./image5/image',num2str(n),'.jpg');   
    frame=read(videoObj,i);           
    imwrite(frame,image_name,'jpg');
    clc;
end

disp('all images are written into directory image');

% aviobj = VideoWriter('33.avi');
% aviobj.FrameRate = 25;
% open(aviobj)
% 
% for i = 1:372
%     disp([num2str(i/frameNumber*100),'%']) 
%     image_file =  ['image5/im_',num2str(i),'.jpg'];
%     des = imread(image_file);
% 
%   writeVideo(aviobj,des);
% end
% close(aviobj)
% disp('视频处理完成');

