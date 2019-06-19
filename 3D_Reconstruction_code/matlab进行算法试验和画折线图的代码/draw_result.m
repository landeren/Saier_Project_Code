close all
clc
type_name = ['xy.txt';'xz.txt';'yz.txt'];
hxzqd_file_root = 'C:/Users/FBJ/Desktop/data/Sofa06/PEFR/flat/result1_type1_';
sxkd_file_root = 'C:/Users/FBJ/Desktop/data/Sofa06/PEFR/flat/result1_type2_';
type =3;
if type == 1
    %读取单个横向准确度的txt文件并显示
    file_name = [hxzqd_file_root,type_name(1,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];

    x=1:1:15; %确定x轴范围

    figure
    plot(x,result_up,'-s',x,result_down,'*-'); %折线绘图
    grid on;
    %axis([1,15,50,100]) %确定x与y轴框图大小
    set(gca,'XTick',[1:1:15]); %改变x轴坐标间隔显示,范围是0-60，这里间隔为3
    %set(gca,'YTick',[80:1:100]) %改变y轴坐标间隔显示，范围是0-50，这里间隔为3
    %str=[repmat('',15,1) num2str(result_up')];
    %text(x,result_up,cellstr(str));
    %str=[repmat('',15,1) num2str(result_down')];
    %text(x,result_down,cellstr(str));
    legend('upper acc','above acc');
    title('boundaries accurace')
    xlabel('range of point cloud') %x坐标的描述
    ylabel('accuracy(%)') %y坐标的描述


    %读取单个横向准确度文件并得出平均值显示
    figure
    result_total = (result_up + result_down)/2;
    plot(x,result_total,'-*');
    grid on;
    set(gca,'XTick',[1:1:15]);
    str = [repmat('',15,1) num2str(result_total',4)];
    text(x,result_total+0.25,cellstr(str));
    legend('upper acc');
    title('total result')
    xlabel('range of point cloud')
    ylabel('accuracy(%)') 
    

elseif type == 2
    %同时读取三个投影面上的结果并显示在同一图上
    file_name = [hxzqd_file_root,type_name(1,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_xy = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_xy = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];
    file_name = [hxzqd_file_root,type_name(2,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_xz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_xz = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];
    file_name = [hxzqd_file_root,type_name(3,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_yz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_yz = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];

    x=1:1:15; 

    figure
    plot(x,(result_up_xy + result_down_xy)/2,'-s',x,(result_up_xz + result_down_xz)/2,'-*',x,(result_up_yz + result_down_yz)/2,'-o'); 
    grid on;
    set(gca,'XTick',[1:1:15]); 
    legend('upper acc','above acc');
    title('Upper and above boundaries accurace')
    xlabel('range of point cloud') 
    ylabel('accuracy(%)') 
    
%同时读取三个投影面上的结果并得出平均结果显示
    figure
    result_total = (result_up_xy + result_down_xy + result_up_xz + result_down_xz + result_up_yz + result_down_yz)/6;
    plot(x,result_total,'-*');
    grid on;
    set(gca,'XTick',[1:1:15]);
    str = [repmat('',15,1) num2str(result_total',4)];
    text(x,result_total+0.25,cellstr(str));
    legend('upper acc');
    title('total result')
    xlabel('range of point cloud')
    ylabel('accuracy(%)') 
    
%读取三个投影面上的竖向跨度并显示在同一图上
    file_name = [sxkd_file_root,type_name(1,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_xy = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    file_name = [sxkd_file_root,type_name(2,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_xz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    file_name = [sxkd_file_root,type_name(3,:)];
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(file_name, '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_yz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
 
    x=1:1:15; 

    figure
    plot(x,result_xy,'-s',x,result_xz,'-*',x,result_yz,'-o'); 
    grid off;
    set(gca,'XTick',[1:1:15]); 
    legend('upper acc','above acc');
    title('Upper and above boundaries accurace')
    xlabel('range of point cloud') 
    ylabel('accuracy(%)') 

%读取三个投影面上的竖向跨度并得出平均结果显示
    figure
    result_total = (result_xy + result_xz + result_yz )/3;
    plot(x,result_total,'-o');
    grid off;
    set(gca,'XTick',[1:1:15]);
    str = [repmat('',15,1) num2str(result_total',4)];
    text(x,result_total+0.25,cellstr(str));
    legend('upper acc');
    title('total result')
    xlabel('range of point cloud')
    ylabel('accuracy(%)') 
    
elseif type == 3
%读取多个文件的三个投影面的准确度平均值结果并显示
axis_name = {'xy.txt','xz.txt','yz.txt'};
model_name = {'/Desk05'};
%model_name = {'/Sofa05'};
%model_name = {'/Sofa06'};
method_name = {'/PEFR','/AKAZE','/KAZE','/ANATOMY','/VisualSFM'};
type_name = {'/flat/result1_type1_','/flat/result1_type2_'};
file_root = 'C:/Users/FBJ/Desktop/data';

result_all_xy=[];
result_all_xz=[];
result_all_yz=[];
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(1))
    file_name = strcat(file_name,axis_name(1))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_xy = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_xy = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];
    temp_average_xy = (result_up_xy+result_down_xy)/2;
    result_all_xy=cat(1,result_all_xy,temp_average_xy);
end
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(1))
    file_name = strcat(file_name,axis_name(2))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_xz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_xz = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];
    temp_average_xz = (result_up_xz+result_down_xz)/2;
    result_all_xz=cat(1,result_all_xz,temp_average_xz);
end
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(1))
    file_name = strcat(file_name,axis_name(3))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');
    result_up_yz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_down_yz = [data1(2),data2(2),data3(2),data4(2),data5(2),data6(2),data7(2),data8(2),data9(2),data10(2),data11(2),data12(2),data13(2),data14(2),data15(2)];
    temp_average_yz = (result_up_yz+result_down_yz)/2;
    result_all_yz=cat(1,result_all_yz,temp_average_yz);
end
    result_all_xy=(result_all_xy+result_all_xz+result_all_yz)/3
    x=1:1:15; 

    figure
    h = plot(x,result_all_xy(1,:),'-p',x,result_all_xy(2,:),'-*',x,result_all_xy(3,:),'-o',x,result_all_xy(4,:),'-x',x,result_all_xy(5,:),'-s'); 
    axh = gca;
    set(axh,'XGrid','off')
    set(axh,'YGrid','on')
    set(gca,'XTick',[1:1:15]); 
    legend('Our method','AKAZE','KAZE','ANATOMY','VisualSFM');
    title('model structure accurace','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
    xlabel('range of point cloud','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
    ylabel('accuracy(%)','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
    
%读取多个文件的三个投影面的跨度平均值结果并显示
result_all_xy=[];
result_all_xz=[];
result_all_yz=[];
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(2))
    file_name = strcat(file_name,axis_name(1))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_up_xy = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_all_xy=cat(1,result_all_xy,result_up_xy);
end
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(2))
    file_name = strcat(file_name,axis_name(2))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_up_xz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_all_xz=cat(1,result_all_xz,result_up_xz);
end
for i=1:1:5
    file_name = strcat(file_root,model_name(1))
    file_name = strcat(file_name,method_name(i))
    file_name = strcat(file_name,type_name(2))
    file_name = strcat(file_name,axis_name(3))
    [data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11,data12,data13,data14,data15] = textread(char(file_name), '%*s%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f','headerlines',3);
    result_up_yz = [data1(1),data2(1),data3(1),data4(1),data5(1),data6(1),data7(1),data8(1),data9(1),data10(1),data11(1),data12(1),data13(1),data14(1),data15(1)];
    result_all_yz=cat(1,result_all_yz,result_up_yz);
end
    result_all_xy=(result_all_xy+result_all_xz+result_all_yz)/3
    x=1:1:15; 

    figure
    plot(x,result_all_xy(1,:),'-p',x,result_all_xy(2,:),'-*',x,result_all_xy(3,:),'-o',x,result_all_xy(4,:),'-x',x,result_all_xy(5,:),'-s'); 
    axh = gca;
    set(axh,'XGrid','off')
    set(axh,'YGrid','on')
    set(gca,'XTick',[1:1:15]); 
    legend('Our method','AKAZE','KAZE','ANATOMY','VisualSFM');
    title('model structure accurace','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
    xlabel('range of point cloud','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
    ylabel('accuracy(%)','FontWeight','demi','FontSize',14,...
    'FontName','Times New Roman');
end
