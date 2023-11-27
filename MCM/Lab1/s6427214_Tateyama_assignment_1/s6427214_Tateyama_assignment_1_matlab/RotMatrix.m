function R = RotMatrix(x, y, z)
    R = [cos(x)*cos(y) cos(x)*sin(y)*sin(z)-sin(x)*cos(z) cos(x)*sin(y)*cos(z)+sin(x)*sin(z);
         sin(x)*cos(y) sin(x)*sin(y)*sin(z)+cos(x)*cos(z) sin(x)*sin(y)*cos(z)-cos(x)*sin(z);
         -sin(y) cos(y)*sin(z) cos(y)*cos(z)];
end