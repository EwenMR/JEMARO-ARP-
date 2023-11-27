function R = ComputeAngleAxis(theta,v)
%Implement here the Rodrigues formula
    I = eye(3,3);
    v_m = [0 -v(1, 3) v(1, 2); 
           v(1, 3) 0 -v(1, 1);
           -v(1, 2) v(1, 1) 0];

    v_m2 = v_m * v' - I;

    R = I + (sin(theta) * v_m) + v_m2 * (1 - cos(theta));
end
