%this code test the effect on matlab for gray-scale boat image with traditional algorithm
close all
clc
tic
originalt = imread('C:\Users\FBJ\Desktop\vgg_oxford_feat_eval\wall\img1.jpg');
original = originalt;
figure(1)
image(original)
title('Input image')
axis image
colormap(gray(256));

distortedt = imread('C:\Users\FBJ\Desktop\vgg_oxford_feat_eval\wall\img6.jpg');
distorted = distortedt;
figure(2)
image(distorted)
title('affine image')
axis image
colormap(gray(256));

PtsOriginal = detectSURFFeatures(original);
PtsDistorted = detectSURFFeatures(distorted);
[featureOri, validOri] = extractFeatures(original,PtsOriginal);
[featureDis, validDis] = extractFeatures(distorted, PtsDistorted);

index_pair = matchFeatures(featureOri, featureDis);
matchedOriginal = validOri(index_pair(:,1));
matchedDistorted = validDis(index_pair(:,2));

figure(6)
showMatchedFeatures(original, distorted, matchedOriginal, matchedDistorted)
title('matched')

imwrite(originalt,'C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/wraped_image/img1.jpg');
imwrite(distortedt,'C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/wraped_image/img2.jpg');

t = toc

