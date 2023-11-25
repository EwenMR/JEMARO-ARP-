function [rot_matrix] = quatToRot(q0,q1,q2,q3)
% quatToRot convert a quaternion into a rotation matrix
    %Covert a quaternion into a full three-dimensional rotation matrix.
    %Input
    %:param Q: A 4 element array representing the quaternion (q0,q1,q2,q3) 
    %Output
    %return: A 3x3 element matrix representing the full 3D rotation matrix. 
    %3x3 rotation matrix
    % q = q0 + xi + yj + zk
    % q = 0.1647 + 0.31583i + 0.52639j + 0.77204k

    % returns the rotation matrix 
      rot_matrix = [ 1 - 2*q2^2 - 2*q3^2  , 2*q1*q2 - 2*q3*q0        , 2*q1*q3 + 2*q2*q0        ;
      2*q1*q2 + 2*q3*q0       ,   1 - 2*q1^2 - 2*q3^2  , 2*q2*q3 - 2*q1*q0        ;
      2*q1*q3 - 2*q2*q0        ,  2*q2*q3 + 2*q1*q0        , 1 - 2*q1^2 - 2*q2^2  ];
end