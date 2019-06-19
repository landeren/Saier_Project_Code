fid=fopen('E:/BaiduNetdiskDownload/data/long4/1.txt','wt'); 
for k = 1:1:290
%     add = '000';
%     if k == 0
%         add = '0000'
%     elseif k < 100
%         add = '000'
%     elseif k <1000
%         add = '00'
%     elseif k<10000
%         add = '0'
%     end;
    fprintf(fid,['/driver_100_30frame/long4/image',num2str(k),'.jpg']);
    fprintf(fid,'\n');
end
fclose(fid);  