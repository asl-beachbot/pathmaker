%Hello! this will plot Bezier curve for n control points 
%This is a replacement of the program 'Parametic Cubic Bezier Curve'
%submitted before ...Bezier for any number of points ...enjoy 

function y = bezier_curve(p) 

	n = length(p)
	n1 = length(p) - 1
	for i=0:1:n1
		sigma(i+1)=factorial(n1)/(factorial(i)*factorial(n1-i));  % for calculating (x!/(y!(x-y)!)) values 
	end
	l=[];
	UB=[];
	for u=0:0.002:1
		for d=1:n
			UB(d)=sigma(d)*((1-u)^(n-d))*(u^(d-1));
		end
		l=cat(1,l,UB);                                      %catenation 
	end
	P=l*p;
	line(P(:,1),P(:,2))
	line(p(:,1),p(:,2))
end
% books for reference on the subject of cad/cam author Roger Adams  ,
% Ibrahim Zeid 
%Prepared by Mechanical engg. student NIT Allahabad , India
% for any questions feel free to mail me at slnarasimhan89@gmail.com
