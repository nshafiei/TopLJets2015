import root;
import pad_layout;

string f_old = "../../PPXZGenerator_debug.root";
string f = "../../ppxzGeneratorValidation.root";

string particles[];
particles.push("X"); 
particles.push("Z"); 

//----------------------------------------------------------------------------------------------------

void MakeOnePlot(string tag, string alabel, string caption="")
{
	NewPad(alabel);
	draw(RootGetObject(f, "before simulation/" + tag), "vl,eb", black);
	draw(RootGetObject(f_old, tag), "vl,eb", red+dashed);
	//draw(RootGetObject(f, "after simulation/" + tag), "vl,eb", green);

	if (caption != "")
		AttachLegend(caption);
}

//----------------------------------------------------------------------------------------------------

string quantities[], q_labels[];
quantities.push("p_T"); q_labels.push("$p_{\rm T}\ung{GeV}$");
quantities.push("p_z"); q_labels.push("$p_z\ung{GeV}$");
quantities.push("p_tot"); q_labels.push("$p\ung{GeV}$");
quantities.push("theta"); q_labels.push("$\th$");
quantities.push("eta"); q_labels.push("$\et$");

string particles[], p_labels[];
particles.push("X"); p_labels.push("X");
particles.push("Z"); p_labels.push("Z");
//particles.push("l_mi"); p_labels.push("$\rm l^-$");
//particles.push("l_pl"); p_labels.push("$\rm l^+$");

for (int qi : quantities.keys)
{
	for (int pti : particles.keys)
	{
		if (pti == 2)
			NewRow();

		MakeOnePlot("h_" + quantities[qi] + "_" + particles[pti], q_labels[qi], p_labels[pti]);
	}

	GShipout(quantities[qi]);
}

//----------------------------------------------------------------------------------------------------

MakeOnePlot("h_m_Z", "$m_{\rm Z}\ung{GeV}$");

MakeOnePlot("h_m_XZ", "$m_{\rm XZ}\ung{GeV}$");

MakeOnePlot("h_p_z_LAB_2p", "$p_z(\hbox{2 protons})\ung{GeV}$");
