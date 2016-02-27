 
img = double(imread('/home/21104511/Documents/master/oim/tp2/image'));
taille = size(img);
histogramme = zeros(256,1);

 imgg = uint8(zeros(taille(1),taille(2)));
 for x=1:taille(1)
     for y=1:taille(2)
         var = uint8((img(x,y,1)+img(x,y,2)+img(x,y,3)) / 3);
         imgg(x,y) = var;
         if(var==0)
             histogramme(1)=histogramme(1)+1;
         else
             histogramme(var+1) = histogramme(var+1)+1;
         end
     end
 end

 
 