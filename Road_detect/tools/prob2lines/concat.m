clc
clear
I=cell(1,64); 

for k=1:1:560
    for j = 1:2
        add = '000';
        if k == 0
            add = '0000'
        elseif k < 100
            add = '000'
        elseif k <1000
            add = '00'
        elseif k<10000
            add = '0'
        end;
    img_name=['.\output\vgg_SCNN_DULR_w9\driver_100_30frame\long6\image',num2str(k),'.result',num2str(j),'.jpg']
    img_name_save = ['.\output\vgg_SCNN_DULR_w9\driver_100_30frame\re6\','result',num2str(k),'.jpg']
    img=imread(img_name);
    I{j}=img;
    end
    D=cell(2,1);
    I{1} = imresize(I{1},[288,800]);
    D{1,1} = double(I{1});
    D{2,1} = double(I{2});
    Dmat=cell2mat(D);
    Dmat=uint8(Dmat);
    imshow(Dmat);
    imwrite(Dmat,img_name_save);
    
end


