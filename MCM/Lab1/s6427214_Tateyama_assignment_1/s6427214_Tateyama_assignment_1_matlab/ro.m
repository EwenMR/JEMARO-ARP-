function [theta, v] = ro(a, b, c)
    theta = sqrt((a^2 + b^2 + c^2));
    v = [a/theta, b/theta, c/theta];
end