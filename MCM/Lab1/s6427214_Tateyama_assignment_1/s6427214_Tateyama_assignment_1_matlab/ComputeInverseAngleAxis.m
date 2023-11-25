function [theta,v] = ComputeInverseAngleAxis(R)
%EULER REPRESENTATION: Given a tensor rotation matrices this function
% should output the equivalent angle-axis representation values,
% respectively 'theta' (angle), 'v' (axis) 
% SUGGESTED FUNCTIONS
    % size()
    % eye()
    % eig()
    % find()
    % abs()
    % det()
    % NB: Enter a square, 3x3 proper-orthogonal matrix to calculate its angle
    % and axis of rotation. Error messages must be displayed if the matrix
    % does not satisfy the rotation matrix criteria.
    
    % Check matrix R to see if its size is 3x3
    if (size(R) == [3,3])
        
        % Check matrix R to see if it is orthogonal
        if (round(R' * R,2) == eye(3))

            % Check matrix R to see if it is proper: det(R) = 1
            if (round(det(R),2) == 1)
                % Compute the angle of rotation
                theta = acos((R(1,1)+R(2,2)+R(3,3)-1)/2);
                % Calculate eigenvalues and eigenvectors of R
                % [v, d] = eig(R);
                % Compute the axis of rotation
                v = ((1 / (2 * sin(theta))) * [R(3,2)-R(2,3); R(1,3)-R(3,1); R(2,1)-R(1,2)]);

            else
              err('DETERMINANT OF THE INPUT MATRIX IS NOT 1')
            end
        else
             err('NOT ORTHOGONAL INPUT MATRIX')
        end
    else
       err('WRONG SIZE OF THE INPUT MATRIX')
    end
end

