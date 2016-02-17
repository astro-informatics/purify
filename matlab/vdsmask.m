function [mask,maskc]=vdsmask(N,M,p)
%vds generates a binary sampling mask based on a variable density
%sampling profile of the form: (1-r).^d + alpha
%If d->infinity and alpha->0 the profile converges to uniform sampling
%Inputs
%N and M: SIze of mask
%p: Coverage percentage


nb_meas=round(p*N*M);
tol=ceil(p*N*M)-floor(p*N*M);
d=1;


[x,y] = meshgrid(linspace(-1, 1, M), linspace(-1, 1, N)); % Cartesian grid
r = sqrt(x.^2+y.^2); r = r/max(r(:)); % Polar grid

alpha=-1;
it=0;
maxit=20;
while (alpha<-0.01 || alpha>0.01) && it<maxit
    pdf = (1-r).^d;
    [new_pdf,alpha] = modifyPDF(pdf, nb_meas);
    if alpha<0
        d=d+0.1;
    else
        d=d-0.1;
    end
    it=it+1;
end



mask = zeros(size(new_pdf));
while sum(mask(:))>nb_meas+tol || sum(mask(:))<nb_meas-tol
    mask = genMask(new_pdf);
end
M1=floor(M/2);
mask(:,1:M1)=0;
% mask1=fliplr(mask);
% mask1=flipud(mask1);
% 
% if ~mod(M,2)
%     M1=M1-1;
% end
% 
% 
% maskc=[zeros(N,1) mask1(:,1:M1) zeros(N,M-M1+1)];


mask = ifftshift(mask);
mask(1,1)=0;
mask(floor(N/2):end,1)=0;
