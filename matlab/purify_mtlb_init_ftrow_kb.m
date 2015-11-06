function row = purify_mtlb_init_ftrow_kb(u,v,x,y,x3,y3,tau1,tau2,Kx,Ky,Nx2,Ny2)
%init_ftrow Initialize each row of the interpolation matrix
%   Detailed explanation goes here

    temp=find(x<=u);
    g=temp(end);
    clear temp
    temp=find(y<=v);
    h=temp(end);
    clear temp
    
    g1=g-Kx/2+1;
    g2=g+Kx/2;
    h1=h-Ky/2+1;
    h2=h+Ky/2;
    
    g11=g1+((Kx/2));
    g22=g2+((Kx/2));
    h11=h1+((Ky/2));
    h22=h2+((Ky/2));
    
    x1=x3(g11:g22);
    y1=y3(h11:h22);
    
    [x2,y2]=meshgrid(x1,y1);
    %R=exp(-(((x2-u)/tau1).^2+((y2-v)/tau2).^2)/2);
    alphax = 2.34*Kx*pi;
    alphay = 2.34*Ky*pi;
    R=besseli(0,alphax*real(sqrt(1-((2*(x2-u)/tau1).^2))))/besseli(0,alphax) * besseli(0,alphay*real(sqrt(1-((2*(y2-v)/tau2).^2))))/besseli(0,alphay);
    %R=R/sum(sum(R));
    %Q=sparse(Ny2,Nx2);
    Q=spalloc(Ny2,Nx2,Kx*Ky);
    
    
    %Foldings for the circular shifts
    
    if (g1<1)&&(h1<1)
        
        g11=2-g1; 
        h11=2-h1;
        g21=g2;
        h21=h2;
        Q(1:h21,1:g21)=R(h11:Ky,g11:Kx);
        
        g12=2-g1; 
        h12=1-h1;
        g22=g2;
        h22=Ny2+h1;
        Q(h22:Ny2,1:g22)=R(1:h12,g12:Kx);
        
        g13=1-g1; 
        h13=1-h1;
        g23=Nx2+g1;
        h23=Ny2+h1;
        Q(h23:Ny2,g23:Nx2)=R(1:h13,1:g13);
        
        g14=1-g1; 
        h14=2-h1;
        g24=Nx2+g1;
        h24=h2;
        Q(1:h24,g24:Nx2)=R(h14:Ky,1:g14);
        
    elseif (g1<1)&&(h1>=1)&&(h2<=Ny2)
        
        g11=1-g1;
        g21=Nx2+g1;
        Q(h1:h2,g21:Nx2)=R(:,1:g11);
        
        g12=2-g1;
        g22=g2;
        Q(h1:h2,1:g22)=R(:,g12:Kx);
        
    elseif (g1<1)&&(h2>Ny2)
        
        g11=2-g1; 
        h11=Ny2-h1+2;
        g21=g2;
        h21=h2-Ny2;
        Q(1:h21,1:g21)=R(h11:Ky,g11:Kx);
        
        g12=2-g1; 
        h12=Ny2-h1+1;
        g22=g2;
        h22=h1;
        Q(h22:Ny2,1:g22)=R(1:h12,g12:Kx);
        
        g13=1-g1; 
        h13=Ny2-h1+1;
        g23=Nx2+g1;
        h23=h1;
        Q(h23:Ny2,g23:Nx2)=R(1:h13,1:g13);
        
        g14=1-g1; 
        h14=Ny2-h1+2;
        g24=Nx2+g1;
        h24=h2-Ny2;
        Q(1:h24,g24:Nx2)=R(h14:Ky,1:g14);
        
    elseif (g1>=1)&&(g2<=Nx2)&&(h1<1)
        
        h11=1-h1;
        h21=Ny2+h1;
        Q(h21:Ny2,g1:g2)=R(1:h11,:);
        
        h12=2-h1;
        h22=h2;
        Q(1:h22,g1:g2)=R(h12:Ky,:);
        
    elseif (g1>=1)&&(g2<=Nx2)&&(h1>=1)&&(h2<=Ny2)
        
        Q(h1:h2,g1:g2)=R;
        
    elseif (g1>=1)&&(g2<=Nx2)&&(h2>Ny2)
        
        h11=Ny2-h1+1;
        h21=h1;
        Q(h21:Ny2,g1:g2)=R(1:h11,:);
        
        h12=Ny2-h1+2;
        h22=Ky-h11;
        Q(1:h22,g1:g2)=R(h12:Ky,:);
        
    elseif (g2>Nx2)&&(h1<1)
        
        g11=Nx2-g1+2; 
        h11=2-h1;
        g21=g2-Nx2;
        h21=h2;
        Q(1:h21,1:g21)=R(h11:Ky,g11:Kx);
        
        g12=Nx2-g1+2; 
        h12=1-h1;
        g22=g2-Nx2;
        h22=Ny2+h1;
        Q(h22:Ny2,1:g22)=R(1:h12,g12:Kx);
        
        g13=Nx2-g1+1; 
        h13=1-h1;
        g23=g1;
        h23=Ny2+h1;
        Q(h23:Ny2,g23:Nx2)=R(1:h13,1:g13);
        
        g14=Nx2-g1+1; 
        h14=2-h1;
        g24=g1;
        h24=h2;
        Q(1:h24,g24:Nx2)=R(h14:Ky,1:g14);
        
    elseif (g2>Nx2)&&(h1>=1)&&(h2<=Ny2)
        
        g11=Nx2-g1+1;
        g21=g1;
        Q(h1:h2,g21:Nx2)=R(:,1:g11);
        
        g12=Nx2-g1+2;
        g22=Kx-g11;
        Q(h1:h2,1:g22)=R(:,g12:Kx);
        
        
    elseif (g2>Nx2)&&(h2>Ny2)
        
        g11=Nx2-g1+2; 
        h11=Ny2-h1+2;
        g21=g2-Nx2;
        h21=h2-Ny2;
        Q(1:h21,1:g21)=R(h11:Ky,g11:Kx);
        
        g12=Nx2-g1+2; 
        h12=Ny2-h1+1;
        g22=g2-Nx2;
        h22=h1;
        Q(h22:Ny2,1:g22)=R(1:h12,g12:Kx);
        
        g13=Nx2-g1+1; 
        h13=Ny2-h1+1;
        g23=g1;
        h23=h1;
        Q(h23:Ny2,g23:Nx2)=R(1:h13,1:g13);
        
        g14=Nx2-g1+1; 
        h14=Ny2-h1+2;
        g24=g1;
        h24=h2-Ny2;
        Q(1:h24,g24:Nx2)=R(h14:Ky,1:g14);
        
    end
    
    row=Q(:)';

end

