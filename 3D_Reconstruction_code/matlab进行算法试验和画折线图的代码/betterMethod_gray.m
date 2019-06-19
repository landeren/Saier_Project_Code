%this code test the effect on matlab for gray-scale image with better algorithm
close all
clc
tic
originalt = imread('C:\Users\FBJ\Desktop\vgg_oxford_feat_eval\graf\img1.jpg');
original = rgb2gray(originalt);
%original = originalt;
figure(1)
image(original)
title('Input image')
axis image
colormap(gray(256));

distortedt = imread('C:\Users\FBJ\Desktop\vgg_oxford_feat_eval\graf\img3.jpg');
distorted = rgb2gray(distortedt);
%distorted = distortedt;
figure(2)
image(distorted)
title('affine image')
axis image
colormap(gray(256));

PtsOriginal = detectFASTFeatures(original,'MinQuality',0.28);
PtsDistorted = detectFASTFeatures(distorted,'MinQuality',0.28);
PtsOriginal = detectFASTFeatures(original);
PtsDistorted = detectFASTFeatures(distorted);
[featureOri, validOri] = extractFeatures(original,PtsOriginal);
[featureDis, validDis] = extractFeatures(distorted, PtsDistorted);
imshow(originalt); hold on;
plot(PtsOriginal); 
imshow(distortedt); hold on;
plot(PtsDistorted);

index_pair = matchFeatures(featureOri, featureDis);
matchedOriginal = validOri(index_pair(:,1));
matchedDistorted = validDis(index_pair(:,2));
figure(3);
showMatchedFeatures(original,distorted,matchedOriginal,matchedDistorted);

[tform, inlierPtsDistorted, inlierPtsOriginal] = estimateGeometricTransform(matchedDistorted, matchedOriginal, 'affine');
outputView = imref2d(size(original));
distorted2 = imread('C:\Users\FBJ\Desktop\vgg_oxford_feat_eval\boat\img3.jpg');
Ir = imwarp(distorted, tform, 'OutputView', outputView);
Irt = imwarp(distortedt, tform, 'OutputView', outputView);

figure(5)
image(Ir)
title('result')
axis image
colormap(gray(256))
imwrite(Irt,'C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/wraped_image/img0.jpg');
imwrite(originalt,'C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/wraped_image/img1.jpg');
imwrite(distortedt,'C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/wraped_image/img2.jpg');

PtsIr = detectSURFFeatures(Ir);
[featureIr, validIr] = extractFeatures(Ir, PtsIr); 
index_pair2 = matchFeatures(featureOri, featureIr);
matchedOriginal2 = validOri(index_pair2(:,1))
matchedIr = validIr(index_pair2(:,2));
figure(6)
showMatchedFeatures(original, Ir, matchedOriginal2, matchedIr)
title('matched2')

t = toc

%do the trans
t = load('C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/2.txt');
A = [0.3040, 1.8400; -1.8489, 0.3371];
for i = 1:609
    t(i,:) = t(i,:) - [930.5244, -560.3394];
    t(i,:) = t(i,:) * A^-1; 
end
t = toc
