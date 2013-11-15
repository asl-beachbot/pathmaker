img = imread('../assets/m.jpg', 'jpg');
img = ~im2bw(img, 0.5);
img_res = img;
bw_rem = img;
img_temp = img;
idx = 0;
z_ = img_temp * 0;
    bw_rem = bwmorph(img_temp,'remove');
imshow(bw_rem);
while ~isequal(img_temp, z_)
    idx = idx + 1;
    bw_rem = bwmorph(img_temp,'remove');
    img_temp = img_temp - bw_rem;

    if idx < 4
        img_res = img_res - bw_rem;
    end
    
    if idx == 5
        idx = 0;
    end
end

imshow(~img_res);