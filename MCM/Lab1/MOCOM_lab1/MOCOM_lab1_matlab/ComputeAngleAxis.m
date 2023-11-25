function R = ComputeAngleAxis(theta,v)
%Implement here the Rodrigues formula
I = eye(3);
h = [0, -v(1,3),v(1,2);
    v(1,3),0,v(1,1);
    -v(1,2), v(1,1), 0];

R= I + h*sin(theta)+h*h'*(1-cos(theta));
end
