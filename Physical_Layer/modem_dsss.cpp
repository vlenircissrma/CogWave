/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///////                            Author: Vincent Le Nir                                         ///////
///////                                 Royal Military Academy                                    ///////
///////           Department Communications, Information Systems & Sensors (CISS)                 ///////
///////                             30, Avenue de la Renaissance                                  ///////
///////                                B-1000 Brussels BELGIUM                                    ///////
///////                                   Tel: +3227426624                                        ///////
///////                                 email:vincent.lenir@rma.ac.be                             ///////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "modem_dsss.h"
#include <itpp/comm/sequence.h>
#include <QVector>
#include <itpp/base/svec.h>
#include <itpp/stat/misc_stat.h>
#include <itpp/config.h>
#include <itpp/base/algebra/eigen.h>
#include <itpp/base/converters.h>



Modem_DSSS::Modem_DSSS()
{
    int degree=7;
    SF=pow(2.0,degree)-1;
    OF=1;
    nb_bits=600;
    Gold gold_sequence(degree);
    gold_sequence.get_family();
    bvec bcode=gold_sequence.shift(SF);
    vcode.set_size(SF);
    for(int i=0;i<SF;i++){
     if(bcode[i]==0)
           vcode[i]=-1;
     else
         vcode[i]=1;
    }
    spread.set_code(vcode);
    /*SF=1;
    vec code="1";
    spread.set_code(code);*/
    mse1=0;
    mse2=0;
    mse3=0;
    mse4=0;
    mse5=0;
    mse6=0;
    mse7=0;
    mse8=0;
    mse9=0;
    mse10=0;
    mse11=0;
    mse12=0;
    phi_true=0;
}



cvec Modem_DSSS::modulate(bvec data_packet)
{
    BPSK bpsk;
    vec mapped_transmited_symbols=bpsk.modulate_bits(data_packet);

    vec transmitted_chips;
    transmitted_chips=spread.spread(mapped_transmited_symbols);
    return to_cvec(transmitted_chips);
}


// Synchronisation --> autocorrélation

bvec Modem_DSSS::demodulate(cvec rx_buff,int &time_offset_estimate,cvec &constellation)
{
    //Calcul du temps symbole Ts (2) (3) (4) (5)
    int T=SF*5;
    int M=rx_buff.size()/T;;
    //int T=rx_buff.size()/M;
    cmat rx_buff_m(M,T);
    rx_buff_m.zeros();

    for (int i=0; i<M;i++)
    {
        rx_buff_m.set_row(i,rx_buff.get(i*T,(i+1)*T-1));
    }


    cvec autocorrelation(T/2);
    autocorrelation.zeros();
    vec squared_autocorrelation(T/2);
    squared_autocorrelation.zeros();
    squared_autocorrelation[0]=0;


    for (int i=1; i<T/2; i++)

    {
        squared_autocorrelation[i]=0;
        for(int j=0;j<M;j++)
        {
            autocorrelation[i]=(1./(T))*sum(elem_mult((rx_buff_m.get_row(j)).get(0,T/2-1),(conj(concat((rx_buff_m.get_row(j)).get(i,T-1),(rx_buff_m.get_row(j)).get(0,i-1)))).get(0,T/2-1)));
            squared_autocorrelation[i]+=(1./(M))*real(autocorrelation[i]*conj(autocorrelation[i]));
        }

    }


    int position_max=max_index(squared_autocorrelation);
    squared_autocorrelation.set(position_max,0);
    int position_max_2=max_index(squared_autocorrelation);

    int Ts;
    bool isdsss=false;

    if (((position_max*2==position_max_2)||(position_max/2==position_max_2))&&(position_max>40))
    {
        Ts=min(position_max,position_max_2);
        isdsss=true;
    }

    if(isdsss==true)
    {


        cout << "DSSS detected " << Ts << endl;
        // Calcul de l'offset temporel (6) en trouvant pour quel theta la norme est maximale?
        int N=rx_buff.size()/Ts;
        cmat R(Ts,Ts);
        R.zeros();
        cmat G(1,Ts);
        G.zeros();
        cmat H(Ts,1);
        H.zeros();
        vector<cmat> R_final(Ts);
        cvec tmp_rx_buff=rx_buff;
        vec frobenius_norm(Ts);
        frobenius_norm.zeros();
        for(int theta=0;theta<Ts;theta++)
        {
            R_final[theta].set_size(Ts,Ts);
            R_final[theta].zeros();

            for (int i=0;i<N;i++)
            {

                    H.set_col(0,tmp_rx_buff.get(i*Ts,(i+1)*Ts-1));
                    G=hermitian_transpose(H);
                    R=H*G;
                    R_final[theta]+=1./N*R;

            }
            tmp_rx_buff=concat(tmp_rx_buff.get(1,N*Ts-1),tmp_rx_buff(0));

            frobenius_norm[theta]=norm(R_final[theta],"fro");

        }
        time_offset_estimate=max_index(frobenius_norm);

        cout << "Time offset " << time_offset_estimate << endl;


        //Calcul de la sequence d'étalement par décomposition en valeurs propres (9) (10)
        vec d(Ts);
        cmat V(Ts,Ts);
        V.zeros();
        d.zeros();
        eig_sym(R_final[time_offset_estimate],d,V);

        cvec spreading_sequence(Ts);
        spreading_sequence.zeros();
        /*vec phase=1./2*arg(elem_mult(V.get_col(Ts-1),V.get_col(Ts-1)));
        for(int i=Ts-1;i>0;i--){
            if(abs(phase(i)-phase(i-1))>pi/2){
                if((phase(i)-phase(i-1))>pi/2)
                    phase.set_subvector(0,phase.get(0,i-1)+pi);
                if((phase(i)-phase(i-1))<-pi/2)
                    phase.set_subvector(0,phase.get(0,i-1)-pi);
            }
        }*/

        //spreading_sequence=elem_mult(V.get_col(Ts-1),exp(to_cvec(zeros(Ts),-phase)));
        spreading_sequence=V.get_col(Ts-1);


        vec binary_spreading_sequence(Ts);
        binary_spreading_sequence.zeros();
        for(int i=0;i<Ts;i++)
        {
         if(real(spreading_sequence[i])<0)
               binary_spreading_sequence[i]=1;
         else
             binary_spreading_sequence[i]=-1;
        }
        cout << "Differences codes étalement " << sum(abs(vcode-binary_spreading_sequence))/2 << endl;

        //Frequency Offset Correction algorithms
        double phi=0;
        double somme=0;
        std::complex<double> somme_cplx=0;
        cvec vector_unmodulated;
        vector_unmodulated.zeros();
        vector_unmodulated=elem_mult(V.get_col(Ts-1),V.get_col(Ts-1));
        //Valeur exacte
        //phi=2*pi*1.0e-2;
        /*//Calcul basé sur l'aire
        phi=-atan(2*sum(phase)/(Ts*Ts));
        mse1=mse1+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la régression linéaire
        vec x(Ts);
        for(int i=0;i<Ts;i++)
            x(i)=i;
        phi=(sum(elem_mult(phase,x))-Ts*mean(x)*mean(phase))/(sum(elem_mult(x,x))-Ts*mean(x)*mean(x));
        mse2=mse2+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de LRP (phase averager) sur le vecteur phase après avoir supprimé la modulation BPSK
        somme=0;
        for(int i=1;i<Ts;i++)
            somme=somme+1./Ts*arg(vector_unmodulated(i)*conj(vector_unmodulated(i-1)));
        phi=somme/2;
        mse3=mse3+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de LRP (linear predictor) sur le vecteur phase après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<Ts;i++)
            somme_cplx=somme_cplx+vector_unmodulated(i)*conj(vector_unmodulated(i-1));
        phi=arg(somme_cplx)/2;
        mse4=mse4+(phi-phi_true)*(phi-phi_true);*/
        //Calcul basé sur la méthode de Kay window (weighted phase averager) sur le vecteur phase après avoir supprimé la modulation BPSK
        somme=0;
        for(int i=1;i<Ts;i++)
            somme=somme+double(6*i*(Ts-i))/(double(Ts)*(Ts*Ts-1))*arg(vector_unmodulated(i)*conj(vector_unmodulated(i-1)));
        phi=somme/2;
        mse5=mse5+(phi-phi_true)*(phi-phi_true);
        /*//Calcul basé sur la méthode de Kay circular (weighted linear predictor) sur le vecteur phase après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<Ts;i++)
            somme_cplx=somme_cplx+double(6*i*(Ts-i))/(double(Ts)*(Ts*Ts-1))*vector_unmodulated(i)*conj(vector_unmodulated(i-1));
        phi=arg(somme_cplx)/2;
        mse6=mse6+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de Parabolic Smoothed Central Finite Difference Estimator sur le vecteur phase après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<Ts;i++)
            somme_cplx=somme_cplx+double(6*i*(Ts-i))/(double(Ts)*(Ts*Ts-1))*vector_unmodulated(i)*conj(vector_unmodulated(i-1))/abs(vector_unmodulated(i))/abs(vector_unmodulated(i-1));
        phi=arg(somme_cplx)/2;
        mse7=mse7+(phi-phi_true)*(phi-phi_true);
        int vector_size=SF*nb_bits;
        vector_unmodulated=elem_mult(rx_buff.get(0,vector_size-1),rx_buff.get(0,vector_size-1));
        //Calcul basé sur la méthode de LRP (phase averager) sur les échantillons reçus après avoir supprimé la modulation BPSK
        somme=0;
        for(int i=1;i<vector_size;i++){
            somme=somme+1./vector_size*arg(vector_unmodulated(i)*conj(vector_unmodulated(i-1)));
        }
        phi=somme/2;
        mse8=mse8+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de LRP (linear predictor) sur les échantillons reçus après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<vector_size;i++){
            somme_cplx=somme_cplx+vector_unmodulated(i)*conj(vector_unmodulated(i-1));
        }
        phi=arg(somme_cplx)/2;
        mse9=mse9+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de Kay window (weighted phase averager) sur les échantillons reçus après avoir supprimé la modulation BPSK
        somme=0;
        for(int i=1;i<vector_size;i++){
            somme=somme+double(6*i*(vector_size-i))/(double(vector_size)*(vector_size*vector_size-1))*arg(vector_unmodulated(i)*conj(vector_unmodulated(i-1)));
        }
        phi=somme/2;
        mse10=mse10+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de Kay circular (weighted linear predictor) sur les échantillons reçus après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<vector_size;i++){
            somme_cplx=somme_cplx+double(6*i*(vector_size-i))/(double(vector_size)*(vector_size*vector_size-1))*vector_unmodulated(i)*conj(vector_unmodulated(i-1));
        }
        phi=arg(somme_cplx)/2;
        mse11=mse11+(phi-phi_true)*(phi-phi_true);
        //Calcul basé sur la méthode de Parabolic Smoothed Central Finite Difference Estimator sur les échantillons reçus après avoir supprimé la modulation BPSK
        somme_cplx=0;
        for(int i=1;i<vector_size;i++){
            somme_cplx=somme_cplx+double(6*i*(vector_size-i))/(double(vector_size)*(vector_size*vector_size-1))*vector_unmodulated(i)*conj(vector_unmodulated(i-1))/abs(vector_unmodulated(i))/abs(vector_unmodulated(i-1));
        }
        phi=arg(somme_cplx)/2;
        mse12=mse12+(phi-phi_true)*(phi-phi_true);*/
        cout << "Frequency offset " << phi/(2*pi) << endl;
        //phi=0;
        cvec fo_corrected_chips(rx_buff.size());
        fo_corrected_chips.zeros();
        for(int k=0;k<fo_corrected_chips.size();k++)
            fo_corrected_chips(k)=rx_buff(k)*exp(std::complex<double>(0,-phi*k));

        //Phase offset Correction
        phi=1./2*arg(1./fo_corrected_chips.size()*sum(elem_mult(fo_corrected_chips,fo_corrected_chips)));
        cvec po_corrected_chips=fo_corrected_chips*exp(std::complex<double>(0,-phi));
        //Time Offset Correction
        cvec to_corrected_chips(rx_buff.size());
        to_corrected_chips.zeros();
        if(time_offset_estimate>0)
            to_corrected_chips=concat(po_corrected_chips.get(time_offset_estimate,rx_buff.size()-1),po_corrected_chips.get(0,time_offset_estimate-1));
        else
            to_corrected_chips=po_corrected_chips;
        //Despreading
        Spread_1d spread2;
        spread2.set_code(binary_spreading_sequence);
        constellation=to_cvec(spread2.despread(real(to_corrected_chips),0),spread2.despread(imag(to_corrected_chips),0));
        double norm=1./constellation.size()*real(sum(elem_mult(constellation,conj(constellation))));
        constellation=1./sqrt(norm)*constellation;
        //cout << received_symbols << endl;
        bvec received_bits(constellation.size());
        for(int i=0;i<constellation.size();i++)
        {
            if(real(constellation(i))<0)
                received_bits(i)=1;
            else
                received_bits(i)=0;
        }
        //cout << received_bits << endl;
        return received_bits;

    }
    else{

        cout << "NO DSSS" << endl;
        return zeros_b(rx_buff.size()/(OF*SF));

    }
}


bool Modem_DSSS::preamble_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    BPSK bpsk;
    vector<char> preamble(3);
    bvec received_bits_inverted;
    received_bits_inverted=received_bits+ones_b(received_bits.size());

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='H';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
        if((preamble_bits==received_bits.get(i,i+24-1))&&(preamble_bits==received_bits.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        if((preamble_bits==received_bits_inverted.get(i,i+24-1))&&(preamble_bits==received_bits_inverted.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits_inverted.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bool Modem_DSSS::ack_detection(bvec received_bits,bvec &received_bits2,int &preamble_start){

    BPSK bpsk;
    vector<char> preamble(3);
    bvec received_bits_inverted;
    received_bits_inverted=received_bits+ones_b(received_bits.size());

    preamble[0]='!';
    preamble[1]='!';
    preamble[2]='A';

    bvec preamble_bits=charvec2bvec(preamble);

    bool preamble_ok=false;
    int i=0;
    preamble_start=0;
    while((i<=int(received_bits.size())-144)&&(preamble_ok==false)){
        if((preamble_bits==received_bits.get(i,i+24-1))&&(preamble_bits==received_bits.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        if((preamble_bits==received_bits_inverted.get(i,i+24-1))&&(preamble_bits==received_bits_inverted.get(i+120,i+144-1))){
            preamble_ok=true;
            received_bits2.set_size(received_bits.size()-i);
            received_bits2=received_bits_inverted.get(i,received_bits.size()-1);
            preamble_start=i;
        }
        i++;
    }

    return preamble_ok;

}

bvec Modem_DSSS::charvec2bvec(vector<char> input){
    int bitsize=sizeof(char)*8;
    bvec output(input.size()*bitsize);
    char bintemp;
    bvec temp(bitsize);
    for(int i=0;i<int(input.size());i++){
            bintemp=input[i];
            for(int j=bitsize-1;j>=0;j--){
                temp[j] = bin(bintemp & 1);
                bintemp = (bintemp >> 1);
            }
            output.replace_mid(i*bitsize,temp);
       }
return output;
}
