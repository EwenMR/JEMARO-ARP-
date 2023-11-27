function myplot(exnum,R,theta,v)

disp(['--------------------------------',num2str(exnum),'--------------------------------'])
disp(['Rotational matrix ex', num2str(exnum)]);
disp(R);

disp(['theta ex',num2str(exnum)]);
disp(theta);

disp(['v ex',num2str(exnum)]);
disp(v);
plotRotation(theta,v,R);