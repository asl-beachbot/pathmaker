function graph = image_to_graph(image_path)
    A = imread(image_path, 'png');

end


clear all

search_direction = 'clockwise'
start = 'O'

A = imread('~/projects/pathmaker/assets/simple_path.png', 'png');



image(A)
% indexed A
startpoint = [0, 0];
im_red = A(:,:,1)';
found_start = false;
for px_line = im_red
    startpoint(1) = startpoint(1) + 1; % y
    startpoint(2) = 0; % x
    for px = px_line'
        disp('Line');
        sprintf('index: %f, %f, value: %f\n', startpoint(1), startpoint(2), px)
        startpoint(2) = startpoint(2) + 1;
        if px == 0
            found_start = true;
            break;
        end
    end
    if found_start
        break;
    end
end

% ~ is blackhole
[x_size, y_size] = size(im_red);

disp(startpoint)

searchpoint(2) = startpoint(1);
searchpoint(1) = startpoint(2);
searching = true;
found_smth = false;
color = 50;
A(searchpoint(1), searchpoint(2), 3) = 255;
A(searchpoint(1), searchpoint(2), 3) = 0;
A(searchpoint(1), searchpoint(2), 3) = 0;

while searching == true
    
    if(color < 255) color = color + 20;
    else color = 50;
    end
        
    for i = -1:1
        pt_x = searchpoint(1) + i;
        if(pt_x < 1 || pt_x > x_size) continue; end;
        for j = -1:1
          pt_y = searchpoint(2) + j;
          % sprintf('x: %d, y: %d checked.', pt_x, pt_y)
          if pt_y > 0 && pt_y < y_size
              if A(pt_x, pt_y, 1) == 0
                  A(pt_x, pt_y, 1) = color;
                  A(pt_x, pt_y, 2) = 200;
                  A(pt_x, pt_y, 3) = 200;
                  sprintf('x: %d, y: %d is now red.', pt_x, pt_y)
                  searchpoint = [pt_x, pt_y];
                  found_smth = true;
              end
          end
        end
    end
    if found_smth == false
        if(searchpoint(2) + 1 < y_size)
            searchpoint(2) = searchpoint(2) + 1;
        else 
            searchpoint(2) = 1;
            if(searchpoint(1) + 1 < x_size)
                searchpoint(1) = searchpoint(1) + 1;
            else
                disp('what the hell');
                break;
            end
        end
    else
        found_smth = false;
    end
end

imwrite(A, 'image_with_path.png');