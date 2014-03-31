A = imread('test4.png', 'png');
A = int32(A(:,:,1));
[x,y] = find(A ~= 255);

gradient_weight = 0.6;

n = 100;
phi = (sqrt(5)-1)/2;
theta = 2*pi*phi*(0:n-1);
rho = (1:n).^phi;
%[x,y] = pol2cart(theta(:),rho(:));
%xy = 10*([x y]-min([x;y]))/(max([x;y])-min([x;y]));
[n, ~] = size(x);
xy = [x,y];
popSize = 60;
numIter = 2e4;
a = meshgrid(1:n);
dmat = reshape(sqrt(sum((xy(a,:)-xy(a',:)).^4,2)),n,n);
mat = zeros(n,n);
for i = 1:n
    for j = 1:n
        mat(i, j) = abs(A(xy(i, 1), xy(i, 2)) - A(xy(j, 1), xy(j, 2)));
        dmat(i, j) = dmat(i, j) + gradient_weight * abs(A(xy(i, 1), xy(i, 2)) - A(xy(j, 1), xy(j, 2)));
    end
end

[optRoute,minDist] = tsp_ga(xy,dmat,popSize,numIter,1,1);
