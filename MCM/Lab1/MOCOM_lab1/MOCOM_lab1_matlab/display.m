function [] = display(theta,v)
aRb = ComputeAngleAxis(theta,v);
% % % 
disp(theta)
disp(v)
disp('aRb ex 1.6:');disp(aRb);
plotRotation(theta,v,aRb);
disp('theta ex 1.6:');disp(theta);
disp('v ex 1.6:');disp(v);