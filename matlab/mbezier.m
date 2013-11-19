function result = mbezier(S)
    [s, l] = size(S);
    % create diagonal matrix
    % (maybe use sparse for this?
    adjusted_s = s - 2;
    diag_1 = zeros(adjusted_s, adjusted_s);
    diag_1(1:adjusted_s+1:end) = 4;
    diag_1(2:adjusted_s+1:end-1) = 1;
    diag_1(adjusted_s+1:adjusted_s+1:end) = 1;
    x = S(:,1);
    y = S(:,2);
    
    B = zeros(s, 2);
    
    B(1,:) = S(1,:);
    B(end,:) = S(end, :);
    
    b_ = S(2:end-1, :);
    
    r = 6 .* S(2:end-1, :);
    r(1, :) = r(1, :) - S(1, :);
    r(end, :) = r(end, :) - S(end, :);
    B(2:end-1, :) = inv(diag_1) * r;
    
    result = zeros(s - 1, 4, 2);
    
    for i = 1:s - 1
        result(i, 1, :) = S(i, :);
        result(i, 2, :) = 2/3.*B(i, :) + 1/3 .* B(i+1, :);
        result(i, 3, :) = 1/3.*B(i, :) + 2/3.*B(i+1, :);
        result(i, 4, :) = S(i + 1, :);
    end
end