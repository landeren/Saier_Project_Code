
pt = 'E:/BaiduNetdiskDownload/data/long4/image (';
ext = ').jpg';
dis = dir([pt ext]);
nms = {dis.name};

for k = 1:1:290
    nm = [pt,num2str(k),ext];
    image = imread(nm);
    image2 = imresize(image,[590,1640]);
    saves = ['E:/BaiduNetdiskDownload/data/long4/image',num2str(k),'.jpg']
    imwrite(image2,saves);
    % 对图像image进行相关操作
end
