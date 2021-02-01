
//////////////////////////////////////////////////////////////////////
// Net.Commerce overridable function template
//       Copyright (C) 2000, IBM Corporation
//       Copyright (C) YYYY, --> Client name
//
//
// Overridable function name:	MassImport
//
// File name:					MassImport.cpp
//
// Function:					lancement du mass import
//
// Details:
//       Called by -	MassImport (command WCS)
//       Arguments -	url : l'adresse de redirection de la commande - String
//						CHEMIN : CHEMIN du mass import à effectuer
//						ORGA : nom de l(organisation pour le mass import à effectuer
//						LPR : langue du marchand principal
//						MAJ_CATORG : mise à jour de la table CATORG
//       Flow      -	CodeRetour : code retour de la fonction - Integer
//						0	 = Chargement OK
//						20005	 = pb avec l'inFile
//						20006	 = pb avec le outFile
//						3	 = pas de paramètre type catalogue ('G' ou 'O') sélectionné
//						4	 = le catalogue prix n'est pas en euro alors que l'organisation a basculé en euro
//						99	 = paramètre du pays manquant dans le fichier catalogue
//						1001 = Organisation inexistante dans Guilweb
//						1002 = Erreur SQL lors de la récupération de l'information ORGPARAM : prix TTC ou HT
//						1101 = Monnaie de l'organisation et du marchand inexistante dans Guilweb
//						1102 = Erreur SQL lors de la récupération de la monnaie de l'organisation et du marchand
//						CHEMINOUT = nom du fichier de sortie
//						LANGUE	 = langue du mass import
//
// Change history:
//       MM/DD/YY - Module created.
//          .
//          .
//          .
//
// Comments:
//       Look for the "-->" string to identify areas where changes
//       or additions are needed.
//
//////////////////////////////////////////////////////////////////////

// Provide overridable function class name
#define CMDNAME MassImport       // --> Change "OFtempl" to your function
#define CMDVEND IBM        // --> Change "Vendor" to your vendor name (eg., IBM)
#define CMDPROD NC       // --> Change "Product" to your product name (eg., NC)
#define CMDVERS 1.0           // --> Change to correct version number


// ---------------------- --> Change as needed -----------------------

// Include other needed headersADD_DEMOGRAPHICS
#include "objects/objects.pch"         // Standard NC includes
// --> Insert other includes here as needed
#include <fstream.h>
#include <string.h>
#include <iostream.h>


//=============================================

#ifdef __TRACE_COMMANDS__
	typedef TraceYes Trace;
#else
	typedef TraceNo Trace;
#endif

static Trace trace("MY_COMMANDS ("__FILE__")");

//=============================================

#if defined(WIN32)
	#define __EXPORT_MODE__ __declspec(dllexport)
#elif defined (AIX)
	#define __EXPORT_MODE__
#endif

// Provide macro to stringize a value
#define MAKESTRING(x) MakeStr2(x)
#define MakeStr2(x) #x

//NVP_names declarations
static const StringWithOwnership NVP_Url("url");
static const StringWithOwnership NVP_CHEMIN("CHEMIN");
static const StringWithOwnership NVP_ORGA("ORGA");
static const StringWithOwnership NVP_LANGUE("LANGUE");
static const StringWithOwnership NVP_PAY("PAY");
static const StringWithOwnership NVP_PAYS("PAYS");
static const StringWithOwnership NVP_CodeRetour("CodeRetour");
static const StringWithOwnership NVP_CHEMINOUT("CHEMINOUT");
static const StringWithOwnership NVP_LPR("LPR");
static const StringWithOwnership NVP_MAJ_CATORG("MAJ_CATORG");
static const StringWithOwnership NVP_IDDB2("IDDB2");
static const StringWithOwnership NVP_PASSDB2("PASSDB2");
static const StringWithOwnership NVP_CATGEN("CATGEN");
static const StringWithOwnership NVP_MERCHANT_LG("MERCHANT_LG");
static const StringWithOwnership NVP_CONVERTIR("CONVERTIR");



// variables for configuration file values
String WCS_Path			= _STR_EMPTY_;		// chemin complet vers CommerceSuite
static ConfFile NCOMMERCE;
static ConfField EXEC_PARAM ("EXEC",false,',');

String	Url				= _STR_EMPTY_,
		ORGA			= _STR_EMPTY_,
		CHEMINOUT		= _STR_EMPTY_,
		LPR				= _STR_EMPTY_,
		PAYS			= _STR_EMPTY_,
		LANGUE			= _STR_EMPTY_,
		MAJ_CATORG		= _STR_EMPTY_,
		IDDB2			= _STR_EMPTY_,
		PASSDB2			= _STR_EMPTY_,
		CATGEN			= _STR_EMPTY_,
		MERCHANT_LG		= _STR_EMPTY_,
		CHEMIN			= _STR_EMPTY_,
		CONVERTIR		= _STR_EMPTY_;

// ---------------------- --> Add additional members as needed -------



// Declare the class
 class __EXPORT_MODE__ MassImport : public NC_Command {

	private:

		static const ClassName _STR_ThisClass;

	public:

		MassImport(void);
		virtual bool Initialize(void);
		virtual ~MassImport(void);
		void operator delete( void* p );
		void* operator new(size_t);
		virtual NC_Command* Clone(void);
		virtual void FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err);
		virtual bool CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources);
		virtual bool Process (const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env);

};

// Tracing variable to identify calls to functions for your class.

const ClassName MassImport::_STR_ThisClass(MAKESTRING(CMDNAME));

//////////////////////////////////////////////////////////////////////
//
// The following creates a single instance of the object and passes its
// address to the overidable function manager for registration.
//
// This happens automatically when the DLL is loaded.
//
// The vendor name, product name, class name and version number are
// obtained from the preprocessor macros defined above.
//
//////////////////////////////////////////////////////////////////////

 static bool X = NC_CommandManager::Register(
                     MAKESTRING(CMDVEND),      // Vendor name
                     MAKESTRING(CMDPROD),      // Product name
                     MAKESTRING(CMDNAME),      // Class name
                     CMDVERS,			      // Version
                     new MassImport  // Addr of object
                );



// -------------------------- No changes needed ----------------------


//////////////////////////////////////////////////////////////////////
// "new" and "delete"
//
// Function:
//       These methods are provided here to insure the memory
//       management functions are consistent for this object.  If
//       they were omitted, since this code is in a DLL, different
//       non-compatible new/delete methods may be invoded.  By
//       including them here, that won't occur.
//
// Arguments:
//       new
//            size - Size of storage needed for object.
//       delete
//            addr - Address of storage to be released.
//
// Return value:
//       new
//            Address of storage.
//       delete
//            None.
//
//////////////////////////////////////////////////////////////////////

   void* MassImport::operator new(size_t size) {
    return(::operator new(size));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MassImport::operator delete(void *addr) {
    ::operator delete(addr);
}


// ---------------------- --> Change all the following as needed -----

//////////////////////////////////////////////////////////////////////
// Constructor
//
// Function:
//       This method initializes the object to its initial state.
//
// Arguments:
//       None.
//
// Return value:
//       None but the object is initialized.
//
//////////////////////////////////////////////////////////////////////

MassImport::MassImport(void) {

// Provide trace
    Trace::Tracer _T(_STR_CONSTRUCTOR, _STR_ThisClass);

// load the ncommerce.ini file to get the path to 'CommerceSuite'
	NCOMMERCE.Load("ncommerce");
//get the EXEC param
	WCS_Path=NCOMMERCE.GetConfValue(EXEC_PARAM,false);

}




//////////////////////////////////////////////////////////////////////
// Destructor
//
// Function:
//       This method destroys the object.
//
// Arguments:
//       None.
//
// Return value:
//       None but the object is destroyed.
//
//////////////////////////////////////////////////////////////////////

MassImport::~MassImport(void)
{

// Provide trace
    Trace::Tracer _T(_STR_DESTRUCTOR, _STR_ThisClass);

}




//////////////////////////////////////////////////////////////////////
// Clone
//
// Function:
//       This method return a copy of the object if it can be run
//       in a multithreaded environment.
//
// Arguments:
//       None.
//
// Return value:
//       Address of the object copy if multithreading can be
//       used.  Otherwise, zero.
//
//////////////////////////////////////////////////////////////////////

NC_Command* MassImport::Clone(void)
{
    NC_Command *pClone = 0;

// Provide trace
    Trace::Tracer _T(FuncName("Clone"), _STR_ThisClass);

// Back to caller
    return(pClone);
}

bool MassImport::Initialize(void) {
	// Trace the function call
	Trace::Tracer T(_STR_Initialize, _STR_ThisClass);
	return true;
}

bool MassImport::CheckParameters(const HttpRequest& Req, HttpResponse& Res, NC_Environment& Env, NC_Environment& Resources) {

	Url				= _STR_EMPTY_;
	ORGA			= _STR_EMPTY_;
	CHEMINOUT		= _STR_EMPTY_;
	LPR				= _STR_EMPTY_;
	PAYS			= _STR_EMPTY_;
	LANGUE			= _STR_EMPTY_;
	MAJ_CATORG		= _STR_EMPTY_;
	IDDB2			= _STR_EMPTY_;
	PASSDB2			= _STR_EMPTY_;
	CATGEN			= _STR_EMPTY_;
	MERCHANT_LG		= _STR_EMPTY_;
	CHEMIN			= _STR_EMPTY_;
	CONVERTIR		= _STR_EMPTY_;

	// Trace the function call
	Trace::Tracer T(_STR_CheckParameters, _STR_ThisClass);

	// CHEMIN
	if (Misc::CheckFieldExistence(Req, NVP_CHEMIN, CHEMIN, false) == false) return false;
	debug << indent << " - CheckParams CHEMIN" << CHEMIN << endl;


	// ORGA
	if (Misc::CheckFieldExistence(Req, NVP_ORGA, ORGA, false) == false) return false;
	debug << indent << " - CheckParams ORGA" << ORGA << endl;


	// Url de redirection
	if (Misc::CheckFieldExistence(Req, NVP_Url, Url, false) == false) return false;
	debug << indent << " - CheckParams Url" << Url << endl;

	// Id de DB2
	if (Misc::CheckFieldExistence(Req, NVP_IDDB2, IDDB2, false) == false) {
		IDDB2<<"Missing";
		return true;
	}

	// password de DB2
	if (Misc::CheckFieldExistence(Req, NVP_PASSDB2, PASSDB2, false) == false) {
		PASSDB2<<"Missing";
		return true;
	}

	// CATGEN s'il s'agit du catalogue général ou non
	if (Misc::CheckFieldExistence(Req, NVP_CATGEN, CATGEN, false) == false) {
		CATGEN<<"Missing";
		return true;
	}
	debug << indent << " - CheckParams CATGEN" << CATGEN << endl;

	// LPR
	if (Misc::CheckFieldExistence(Req, NVP_LPR, LPR, false) == false) return false;
	debug << indent << " - CheckParams LPR" << LPR << endl;

	// MAJ_CATORG
	if (Misc::CheckFieldExistence(Req, NVP_MAJ_CATORG, MAJ_CATORG, false) == false) return false;
	debug << indent << " - CheckParams MAJ_CATORG" << MAJ_CATORG << endl;

	// MERCHANT_LG
	if (Misc::CheckFieldExistence(Req, NVP_MERCHANT_LG, MERCHANT_LG, false) == false) return false;
	debug << indent << " - CheckParams MERCHANT_LG" << MERCHANT_LG << endl;

	// PAYS
	if (Misc::CheckFieldExistence(Req, NVP_PAYS, PAYS, false) == false) return false;
	debug << indent << " - CheckParams PAYS" << PAYS << endl;

	// CONVERTIR
	if (Misc::CheckFieldExistence(Req, NVP_CONVERTIR, CONVERTIR, false) == false) return false;
	debug << indent << " - CheckParams CONVERTIR" << CONVERTIR << endl;
	return true;

}

//////////////////////////////////////////////////////////////////////
// FailedRegistration
//
// Function:
//       This method is called when the registration process does not
//       complete sucessfully.
//
// Arguments:
//       RegID - Registration identifier (eg., MyFunction_1.0(IBM,NC)
//       Err   - Address of error type.
//
// Return value:
//       None.
//
//////////////////////////////////////////////////////////////////////

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MassImport::FailedRegistration(NC_RegistrationID& RegID, const ErrorMsg_Reg* Err) {

	// Trace the function call
	Trace::Tracer T(_STR_FailedRegistration, _STR_ThisClass);

	if (Err == &_ERR_REG_UNEXPECTED_OBJ) {
		error << indent << "ERROR: MassImport not accepted by the manager" << endl;
	} else if (Err == &_ERR_REG_DUPE_ID) {
		error << indent << "ERROR: MassImport: another command with the same name has "
			  << "already been registered" << endl;
	} else if (Err == &_ERR_REG_OBJ_NOT_FROM_DLL) {
		error << indent << "ERROR: MassImport is packaged in a different DLL then the one "
			  << "registered in the DB" << endl;
	} else {
		error << indent << "ERROR: MassImport unknown registration error" << endl;
	}

	error << indent << "ERROR: MassImport's registration failed" << endl;

}





//////////////////////////////////////////////////////////////////////
// Process
//
// Function:
//       This method is called by the function manager when a request
//       must be processed.
//
// Arguments:
//       Req  - Reference to the request object.
//       Resp - Reference to the response object.
//       Env  - Reference to the environment object.
//
// Return value:
//        true: Function completed.
//       false: Function failed.
//
//////////////////////////////////////////////////////////////////////

    bool
MassImport::Process(
    HttpRequest const &Req,
    HttpResponse &Resp,
    NC_Environment &Env
) {

 	// variables
	long		CodeRet			=	0;
	long 		Error;
	short		numfound		=	0;
	int			fait			=	0;
	String 		CodeRetour		= _STR_EMPTY_,
				MERCHANT_SD		= _STR_EMPTY_,
				Sgrfnbr			= _STR_EMPTY_,
				Sgfield1		= _STR_EMPTY_,
				Mecur			= _STR_EMPTY_,
				MONNAIE			= _STR_EMPTY_,
				OPTYP			= _STR_EMPTY_;
	char		*fichierin		=	NULL;
	char		*fichierout		=	NULL;
	char 		*environment;
	FILE		*FichierIN, *FichierOUT;
	String		Buffer					=_STR_EMPTY_;
	String		FicInBuffer				=_STR_EMPTY_;
	String		StrTemp					=_STR_EMPTY_;
	String		catmodifiee				=_STR_EMPTY_;
	String		catmodifiee2			=_STR_EMPTY_;
	int			Count;

	NVPHash 	NVP;

	Row SqlRow;										// Lignes retournées par la requete SQL
	String Statement = _STR_EMPTY_;

	char 		*buffer2		=	NULL;			// tampon de lecture ligne par ligne du fichier d'entrée

	static bool readBuffer;							// flag de lecture de fichier in dans le tampon FicInBuffer

	long		ficInSize;							// taille en octets de fichierin

	int			i;

	String 		CommandMassEncap = _STR_EMPTY_;		// chemin de l'executable qui encapsule massimpt
	String 		LigneDeCommande	 = _STR_EMPTY_;		// Ligne de commande avec les paramètres de l'executable qui encapsule massimpt

	int			cpid;								// Child Process Id de l'executable qui encapsule massimpt
	int			fpid;

	static char	theSlash;

	debug << indent << " - Trace"<<endl;

	// Provide trace
    Trace::Tracer _T(_STR_Process, _STR_ThisClass);

	debug << indent << " - Variables"<<endl;

	int		lInt = 0;
	char 	*lTemp;

	lTemp = new char[WCS_Path.Len()];
	if ( lTemp == NULL )
	{
		CodeRet=10000;
		debug << "Erreur d'allocation mémoire (variable lTemp)"<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

	debug << WCS_Path << endl;
	strcpy(lTemp, (char *)WCS_Path.c_str());
	String::Iterator WCS = WCS_Path.getIterator();
	WCS.Seek("CommerceSuite");
	if (WCS.Pos() != WCS_Path.Len())
	{
		lTemp[WCS.Pos()+13]='\0';
		WCS_Path=_STR_EMPTY_;
		WCS_Path << lTemp;// << endl;
	}
	debug << "WCS.pos() = " << WCS.Pos() << endl;
	debug << "WCS_Path.Len() = " << WCS_Path.Len() << endl;
	debug << "WCS_PATH = " << WCS_Path << endl;

	if ( lTemp != NULL)
 		delete lTemp;


	// test de l'environnement
	environment = new char[125];
	if ( environment == NULL )
	{
		CodeRet=10000;
		debug << "Erreur d'allocation mémoire (variable environment)"<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	environment = getenv( "windir" );

	debug  << "ENV "<< environment <<endl;

	CHEMINOUT		= _STR_EMPTY_;
	debug << indent << " - CHEMINOUT "<<CHEMINOUT<<endl;
	fichierin = new char[125];
	if ( fichierin == NULL )
	{
		CodeRet=10000;
		debug << "Erreur d'allocation mémoire (variable fichierin)"<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	strcpy(fichierin,(char*)CHEMIN.c_str());
	debug << indent << " - fichierin "<<fichierin<<endl;

	// Ouverture du fichier d'entrée du MassImport.cpp
	FichierIN = fopen(fichierin,"r");

	if ( FichierIN == NULL)
	{
		debug << indent << " - mauvaise ouverture du fichier inFile"<<endl;
		CodeRet=20005;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

	theSlash = OS::Slash();

	CHEMINOUT << WCS_Path << theSlash << "xml" << theSlash << "massimpt" << theSlash << "nouveau_" << ORGA << ".txt" ;

	fichierout= new char[125];
	if ( fichierout == NULL )
	{
		CodeRet=10000;
		debug << "Erreur d'allocation mémoire (variable fichierout)"<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	strcpy(fichierout,(char*)CHEMINOUT.c_str());
	debug << indent << " - fichierout "<<fichierout<<endl;

	// Ouverture du fichier de sortie du MassImport.cpp à destination du massimptencap.exe
	FichierOUT = fopen(fichierout, "w");
	if ( FichierOUT == NULL )
	{
		debug << indent << " - mauvaise ouverture du fichier outFile"<<endl;
		CodeRet=20006;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

// Test d'existence du paramètre type catalogue spécifiant s'il s'agit d'un catalogue général ou d'organisation

	if (CATGEN=="Missing")
	{
		debug << indent << " - pas de paramètre type catalogue"<<endl;
		CodeRet=3;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

// Test d'existence du nom d'utilisateur DB2 dans le formulaire de saisie. Le test de validité se fait dans la commande massimpt.exe

	if (IDDB2=="Missing")
	{
		debug << indent << " - utilisateur DB2 non renseigné"<<endl;
		CodeRet=1;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

// Test d'existence du mot de passe DB2 dans le formulaire de saisie. Le test de validité se fait dans la commande massimpt.exe

	if (PASSDB2=="Missing")
	{
		debug << indent << " - mot de passe DB2 non renseigné"<<endl;
		CodeRet=2;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

////////////////////////////////////////////////////////////////////////////////////////
// Récupération de l'information ORGPARAM : prix TTC ou HT
////////////////////////////////////////////////////////////////////////////////////////

	DataBase &DB = (DataBase&)    // Database reference
		*Misc::CheckEnvironmentVariable
			(Env,NC_Environment::_VAR_MainDatabase);

// Initialisation Statement
	Statement.Clean();
	debug << indent << " -récupération de l'information ORGPARAM : prix TTC ou HT"
		  << endl;

	// Construction de la requête SQL
	Statement << "select optype,sgrfnbr from orgparam, shopgrp where OPSHGNBR=sgrfnbr and sgname='"<<ORGA<<"'";

	// Execution de la requête et récupération des paramètres
	SQL Sql(DB, Statement);

	Count = 0;
	while ((Error = Sql.getNextRow(SqlRow)) == ERR_DB_NO_ERROR)
	{
		OPTYP=SqlRow.getCol("optype").Trim();
		debug << indent << "optype=" <<OPTYP<<endl;
		Sgrfnbr=SqlRow.getCol("sgrfnbr").Trim();
		debug << indent << "sgrfnbr=" <<Sgrfnbr<<endl;
		Count++;
	}
	if ( Count != 1 ) // vérification de l'existence de l'organisation
	{
		CodeRet=1001;
		debug << " - caractéristiques de l'organisation invalide ou organisation inexistante"<<ORGA<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	if ( (Error != ERR_DB_NO_ERROR) && (Error != ERR_DB_NO_DATA) )          // erreur SQL
	{
		Sql.ReportError();
		CodeRet=1002;
		debug << indent << "Erreur SQL lors de la récupération de l'information ORGPARAM : prix TTC ou HT"
				  <<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

////////////////////////////////////////////////////////////////////////////////////////
// Récupération de la monnaie de l'organisation et de la monnaie du marchand
////////////////////////////////////////////////////////////////////////////////////////

// Initialisation
	Statement.Clean();
	debug << indent << " -récupération de la monnaie de l'organisation et de la monnaie du marchand"
		  << endl;

	// Construction de la requete SQL
	Statement << "select sgfield1,mecur from shopgrp,merchant where sgmenbr=merfnbr and sgname='"<<ORGA<<"'";

	// Execution de la requete et récupération des parametres
	SQL SqlM(DB, Statement);

	Count = 0;
	while ((Error = SqlM.getNextRow(SqlRow)) == ERR_DB_NO_ERROR)
	{
		Sgfield1=SqlRow.getCol("sgfield1").Trim();
		debug << indent << "sgfield1=" <<Sgfield1<<endl;
		Mecur=SqlRow.getCol("mecur").Trim();
		debug << indent << "mecur=" <<Mecur<<endl;
		Count++;
	}
	if ( Count != 1 ) // vérification de l'existence de la monnaie de l'organisation et du marchand
	{
		CodeRet=1101;
		debug << "Monnaie de l'organisation et du marchand inexistante -> "<<ORGA<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	// Basic error checking.
	if ( Error != ERR_DB_NO_ERROR && Error != ERR_DB_NO_DATA )
	{
		SqlM.ReportError();
		CodeRet = 1102;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}


////////////////////////////////////////////////////////////////////////////////////////
// Début écriture du fichier de sortie OUTFILE (nouveau_ORGA.txt)
////////////////////////////////////////////////////////////////////////////////////////
// Epuration du code:
//	1. Calcul de la taille de fichierin pour l'allocation mémoire de la variable buffer2 utilisée pour
//	   	lire ligne par ligne le fichier.
//	2. Suppression des char* catmodif, catmodif2 et buffer

	readBuffer = OS::NC_ReadFile(fichierin, FicInBuffer);

	debug << "Code retour NC_ReadFile : " << readBuffer << endl;

	if ( readBuffer != 1 )
	{
		CodeRet=20001;
		debug << "Erreur de lecture du fichier inFile : " << fichierin << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

	debug << "Nombre d'octets de fichierin ( FicInBuffer.Len() ): " << FicInBuffer.Len() << endl;

	ficInSize = (long) FicInBuffer.Len();

	FicInBuffer.Clean();

	Buffer.Resize(ficInSize);

// chaines à remplacer

	catmodifiee	<< "cat_"<<ORGA<<"@cat";
	catmodifiee2	<< "COMPTECLIENT;"<<ORGA;

	debug << indent << " - textes à modifier : catmodifiee "<<catmodifiee <<endl;

	//buffer2 = (char *) malloc(sizeof(char)*ficInSize);
	buffer2 = new char[100];
	if ( buffer2 == NULL )
	{
		CodeRet=10000;
		debug << "Erreur d'allocation mémoire (variable C++ buffer2)"<<endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

    String PAY = _STR_EMPTY_;
	String Chaine;
	String Pays;

// Lecture 1ème ligne du fichier (#ROWDELIMITER;$)
	if ( fgets(buffer2, 100, FichierIN) == NULL )
	{
		CodeRet=20001;
		debug << "Erreur de lecture 1ème ligne du fichier inFile : " << fichierin << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	fputs(buffer2,FichierOUT);

	if ( ferror(FichierOUT) )
	{
		CodeRet=20002;
		debug << "Erreur d'écriture 1ème ligne dans fichier outFile : " << fichierout << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

// Lecture 2ème ligne du fichier (langue et pays)
	if ( fgets(buffer2, 100, FichierIN) == NULL )
	{
		CodeRet=20001;
		debug << "Erreur de lecture 2ème ligne du fichier inFile : " << fichierin << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	Buffer << buffer2;
	StringIterator Iterat1(&Buffer);
	Iterat1.Seek("#STORE;");
	Iterat1+=7;
	String::Iterator Iterat1LANGUE=Iterat1;
	Iterat1LANGUE+=3;
	String::Iterator Iterat2=Iterat1;
	Iterat2.Seek("$#ROWDELIMITER;");
	String::Iterator Iterat2LANGUE=Iterat2;
	Iterat2LANGUE-=3;

// Test du pays
	if(Iterat2.Len()!=Iterat2.Pos())
	{
		String::Iterator K=Pays.getIterator();
		Pays.Copy(K,Iterat1, Iterat2LANGUE);
		String::Iterator H=Chaine.getIterator();
		Chaine.Copy(H,Iterat1LANGUE, Iterat2);
		PAY<<Pays;
		debug << indent << " coucou les amis : PAY"<<PAY<<endl;
		debug << indent << " coucou les amis : PAYS"<<PAYS<<endl;

		if(PAY!=PAYS)
		{
			debug << indent << " - paramètre du pays de l'organisation différent de celui du fichier catalogue" << endl;
			CodeRet=99;
			CodeRetour<<CodeRet;
			NVP.Add(NVP_CodeRetour, CodeRetour);
			Resp.Redirect(Req,NVP_Url,NVP);
			return true;

		}

		// Si langue non renseignée dans le fichier, on met la langue du marchand principal (=pays)
		if(LANGUE=="")
			LANGUE<<Chaine;
		debug << indent << " hello world : PAY"<<PAY<<endl;
		debug << indent <<  " hello world : LANGUE"<<LANGUE<<endl;
	}

	fputs(Buffer.c_str(),FichierOUT);

	if ( ferror(FichierOUT) )
	{
		CodeRet=20002;
		debug << "Erreur d'écriture dans fichier outFile : " << fichierout << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	Buffer.Clean();


//traitement des autres lignes (non obligatoires au bon fonctionnement du programme massimpt.exe)
//modif 07/11/2000

	i=0;

	long j=0;

	Buffer.Resize(100);

	while ( fgets(buffer2, 100, FichierIN) != NULL )
	{
		j++;

		//debug << indent << "N° Ligne = " << j << indent << " ; Largeur Ligne = " << (long) strlen(buffer2) << "caractères" << endl;

		//debug << indent << buffer2 << endl;

		Buffer << buffer2;

		if( ferror( FichierIN ) )
		{
			CodeRet=20001;
			debug << "Erreur de lecture du fichier inFile : " << fichierin << endl;
			CodeRetour<<CodeRet;
			NVP.Add(NVP_CodeRetour, CodeRetour);
			Resp.Redirect(Req,NVP_Url,NVP);
			return true;
		}

		String::Iterator K=Buffer.getIterator();
		Buffer.Replace(K, "@cat",catmodifiee.c_str(), true);

		String::Iterator J=Buffer.getIterator();
		Buffer.Replace(J, "COMPTECLIENT;",catmodifiee2.c_str(), true);


		StringIterator Inserer2(&Buffer);
		Inserer2.Seek("#PRODPRCS;");
		Inserer2+=10;

		if(Inserer2.Pos()!=Inserer2.Len())
		{
			String::Iterator Inserer3=Inserer2;
			Inserer3.Seek(";");
			Inserer3+=1;
			Buffer.Insert(Inserer3,&ORGA);

			String::Iterator PRI=Inserer3;
			PRI.Seek(";");
			String::Iterator DELIMITER=PRI;
			DELIMITER+=1;
			DELIMITER.Seek(";");
			String::Iterator TAX=DELIMITER;
			TAX+=1;
			TAX.Seek(";");

			if ( i == 0 )
			{
				String::Iterator DEBMONNAIE=TAX;
				DEBMONNAIE+=1;
				String::Iterator FINMONNAIE=TAX;
				FINMONNAIE+=4;

				debug << indent << " - DEBMONNAIE "<<DEBMONNAIE.Pos() <<endl;
				debug << indent << " - FINMONNAIE "<<FINMONNAIE.Pos() <<endl;

				String::Iterator M=MONNAIE.getIterator();
				MONNAIE.Copy(M,DEBMONNAIE,FINMONNAIE);
				debug << indent << "Monnaie :" << MONNAIE << endl;

				i++;
			}

			String PRIX;
			String TAXE;

			debug << indent << " - PRI "<<PRI.Pos() <<endl;
			debug << indent << " - DELIMITER "<<DELIMITER.Pos() <<endl;
			debug << indent << " - TAX "<<TAX.Pos() <<endl;

			String::Iterator K=PRIX.getIterator();
			String::Iterator L=TAXE.getIterator();
			String::Iterator DELIMITER0=PRI;
			DELIMITER0+=2;

			String::Iterator DELIMITER2=DELIMITER;
			DELIMITER2+=1;

			PRIX.Copy(K,DELIMITER0,DELIMITER);
			TAXE.Copy(L,DELIMITER2,TAX);

			debug << indent << " hello world : TAXE"<<TAXE<<endl;
			debug << indent <<  " hello world : PRIX"<<PRIX<<endl;

			double prix, taxe;
			PRIX.getVal(prix);
			TAXE.getVal(taxe);
			double result;

			if(OPTYP=="TTC")
				result=prix*(1+(taxe/10000));

			if(OPTYP=="HT")
				result=prix;

			debug << indent << " taxe"<<taxe<<endl;
			debug << indent << " prix"<<prix<<endl;
			debug << indent << " taxe/100"<<taxe/100<<endl;
			debug << indent << " hello world : result"<<result<<endl;

			String RESULT= _STR_EMPTY_;
			RESULT<<";"<<result<<";"<<Mecur;
			Buffer.Delete(PRI,28);
			Buffer.Insert(PRI,&RESULT);
		}

		fputs(Buffer.c_str(),FichierOUT);

		if ( ferror(FichierOUT) )
		{
			CodeRet=20002;
			debug << "Erreur d'écriture dans fichier outFile : " << fichierout << endl;
			CodeRetour<<CodeRet;
			NVP.Add(NVP_CodeRetour, CodeRetour);
			Resp.Redirect(Req,NVP_Url,NVP);
			return true;
		}
		Buffer.Clean();
	}

	//fin modif

	Buffer << endl;
	Buffer << "#MASSIMPT;"<<Sgrfnbr<<";1;FIN IMPORT CATALOGUE";

	fputs(Buffer.c_str(),FichierOUT);

	if ( ferror(FichierOUT) )
	{
		CodeRet=20002;
		debug << "Erreur d'écriture dans fichier outFile : " << fichierout << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}
	Buffer.Clean();

	if (buffer2 != NULL)
		free(buffer2);

	if(fclose(FichierIN))
	{
		CodeRet=20003;
		debug << indent << " - FichierIN mal fermé : "<< fichierin << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

	if(fclose(FichierOUT))
	{
		CodeRet=20004;
		debug << indent << " - FichierOUT mal fermé : "<< fichierout << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		return true;
	}

////////////////////////////////////////////////////////////////////////////////////////
// Fin écriture du fichier de sortie OUTFILE (nouveau_ORGA.txt)
////////////////////////////////////////////////////////////////////////////////////////

	MERCHANT_SD << PAYS << "_" << LANGUE;
	// Initialisation
	Count = 0;
	Statement.Clean();
	debug << indent << " -test mise à jour nécessaire"
		  << endl;

	// Construction de la requete SQL
	Statement << "SELECT DISTINCT CPCGNBR FROM CGPRREL"
			  << " WHERE CPCGNBR IN (SELECT CGRFNBR FROM CATEGORY WHERE CGMENBR="
			  << "(SELECT MERFNBR FROM MERCHANT WHERE MESTNAME='" << MERCHANT_SD << "')"
			  << " AND CGNBR LIKE 'cat_" << ORGA << "@cat%')"
			  << endl;

	// Execution de la requete et récupération des parametres
	SQL Sql5(DB, Statement);

	Count = 0;
	while ((Error = Sql5.getNextRow(SqlRow)) == ERR_DB_NO_ERROR)
		++Count;

	if ((Error != ERR_DB_NO_ERROR) && (Error != ERR_DB_NO_DATA))          // erreur SQL
	{
			Sql5.ReportError();
			CodeRet=1200;
			debug << indent << "Erreur SQL lors du test mise à jour nécessaire"
					  <<endl;
			CodeRetour<<CodeRet;
			NVP.Add(NVP_CodeRetour, CodeRetour);
			Resp.Redirect(Req,NVP_Url,NVP);
			return true;
	}


debug	<< indent << "TRACE 1" << endl;


	// Si c'est un catalogue général, on ne désactive pas les articles dans CGPRREL (on ne rentre pas dans la boucle)
	// Count=0: pas de catalogue défini pour l'organisation (on ne rentre pas dans la boucle)

	if(CATGEN=="N" && Count!=0)
	{
		// Partie en commentaire (08/08/01) car les comptes clients ne figurent pas toujours dans l'Infocli
		//if(MERCHANT_LG==MERCHANT_SD) {
			//Statement.Clean();
			//debug << indent << " -update compteclient"
			//	  << endl;

			// Construction de la requete SQL
			//Statement << "UPDATE COMPTECLIENT SET CCPUB=2"
			//		  << " WHERE CCSHGNBR="<<Sgrfnbr
			//		  << endl;

			// Execution de la requete et récupération des parametres
			//SQL Sql3(DB, Statement);

			// Traitement des erreurs
			//if (Sql3.getSQLrc() == ERR_DB_NO_DATA) {
			//	Sql3.ReportError();
			//} ;
		//};

		Statement.Clean();
		debug << indent << " -update cgprrel"
			  << endl;

		// Désactivation des articles du catalogue
		// Construction de la requete SQL
		Statement << "UPDATE CGPRREL SET CPSEQNBR=2"
				  << " WHERE CPCGNBR IN (SELECT CGRFNBR FROM CATEGORY WHERE CGMENBR="
				  << "(SELECT MERFNBR FROM MERCHANT WHERE MESTNAME='" << MERCHANT_SD << "')"
				  << " AND CGNBR LIKE 'cat_" << ORGA << "@cat%')"
				  << endl;

		// Execution de la requête et récupération des paramètres
		SQL Sql4(DB, Statement);

		Error = Sql4.getSQLrc();

		if ((Error != ERR_DB_NO_ERROR) && (Error != ERR_DB_NO_DATA))          // erreur SQL
		{
				Sql4.ReportError();
				CodeRet=1300;
				debug << indent << "Erreur SQL (" << Error << ") lors de la requête de désactivation des articles du catalogue"
						  <<endl;
				CodeRetour<<CodeRet;
				NVP.Add(NVP_CodeRetour, CodeRetour);
				Resp.Redirect(Req,NVP_Url,NVP);
				DB.Rollback();
				return true;
		}

	}

	if(MAJ_CATORG=="1")
	{

		// Initialisation
		Statement.Clean();
		debug << indent << " -creation d'une organisation"
			  << endl;

		// Construction de la requete SQL
		Statement << "INSERT INTO ORGCAT (OCCAT, OCORG)"
				  << "VALUES ('cat_" << ORGA <<"', '" << ORGA<<"')"
				  << endl;

		// Execution de la requete et récupération des parametres
		SQL SqlOC(DB, Statement);

		// Traitement des erreurs
		Error = SqlOC.getSQLrc();

		if ( Error != ERR_DB_NO_ERROR )          // erreur SQL
		{
				SqlOC.ReportError();
				CodeRet=1400;
				debug << indent << "Erreur SQL (" << Error << ") lors de la requête de création de l'organisation"
						  << ORGA << endl;
				CodeRetour<<CodeRet;
				NVP.Add(NVP_CodeRetour, CodeRetour);
				Resp.Redirect(Req,NVP_Url,NVP);
				DB.Rollback();
				return true;
		}
	}

	if ( i > 0 )
	{
		if ( (Sgfield1!=MONNAIE) && (MONNAIE=="EUR") )
		{
			// on redirige l'URL pour faire les conversions en euro
			CONVERTIR= _STR_EMPTY_;
			CONVERTIR<<"O";
		}

		if ( (Sgfield1!=MONNAIE) && (MONNAIE!="EUR") )
		{
			// le catalogue prix n'est pas en euro alors que l'organisation a basculé en euro
			debug<<indent<<"Erreur - Le catalogue prix n'est pas en euro alors que l'organisation a basculé en euro"<<endl;
			CodeRet=4;
			CodeRetour<<CodeRet;
			NVP.Add(NVP_CodeRetour, CodeRetour);
			Resp.Redirect(Req,NVP_Url,NVP);
			DB.Rollback();
			return true;
		}

	}

debug	<< indent << "TRACE 2" << endl;

if (fichierin != NULL)
	delete fichierin;

if (fichierout != NULL)
	delete fichierout;

catmodifiee.Clean();

catmodifiee2.Clean();


debug	<< indent << "TRACE 3" << endl;


//************** lancement du massimport ********************
// modif Florian 27/10/2000
//***********************************************************

// préparation de la table de massimport : on enleve la ligne propre a l'organisation si elle existe
	Statement.Clean();
	debug << indent << " -preparation de la table de massimport"
		  << endl;

	// Construction de la requete SQL
	Statement << " DELETE FROM MASSIMPT"
			  << " WHERE MSORG = "<<Sgrfnbr
			  << endl;

	// Execution de la requete et récupération des parametres
	SQL Sql2(DB, Statement);

	// Traitement des erreurs
	Error = Sql2.getSQLrc();

	if ( (Error != ERR_DB_NO_ERROR) && (Error != ERR_DB_NO_DATA) )          // erreur SQL
	{
		Sql2.ReportError();
		CodeRet=1500;
		debug << indent << "Erreur SQL (" << Error << ") lors de la requête de préparation de la table de massimport"
				  << endl;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		DB.Rollback();
		return false;
	}


debug	<< indent << "TRACE 4" << endl;

debug << "preparation du lancement de l'encapsuleur"<<endl;

#ifdef WIN32

	CommandMassEncap << WCS_Path << theSlash << "xml" << theSlash << "massimpt" << theSlash << "massimptencap.exe";
	LigneDeCommande << CommandMassEncap << " " << WCS_Path << " 1 " << ORGA << " " << IDDB2 << " " << PASSDB2;

#else

	CommandMassEncap << WCS_Path << theSlash << "xml" << theSlash << "massimpt" << theSlash << "massimptencap";
	LigneDeCommande << CommandMassEncap << " " << WCS_Path << " 0 " << ORGA << " " << IDDB2 << " " << PASSDB2;

#endif

	debug << "ligne de commande : "<< LigneDeCommande <<endl;

#ifdef WIN32

	// lancement du massimport en tache de fond avec l'exe d'encapsulation
	debug << indent << "CommandMassEncap = " << CommandMassEncap << endl;
	//test = _spawnvp(_P_NOWAIT,(char*) CommandMassEncap.c_str(), args);

	/*Code avec WinExec()

	UINT RETCODE;

	RETCODE = WinExec((LPCTSTR) LigneDeCommande.c_str(), SW_SHOWNORMAL);

	if ( RETCODE < 31 ) //If the function succeeds, the return value is greater than 31
	{
		debug << indent << "Erreur de lancement du programme : " << CommandMassEncap << endl;
		CodeRet=15000;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		DB.Rollback();
		return true;
	}

	debug << indent << "RETCODE = " << (int) RETCODE << endl;

	*/

	// Code avec CreateProcess()

	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	memset(&sInfo,0,sizeof(sInfo));
	sInfo.cb = sizeof(sInfo);
	sInfo.wShowWindow = SW_MAXIMIZE;
	sInfo.dwFlags = STARTF_USESHOWWINDOW;


	if ( CreateProcess ( NULL, (char *) LigneDeCommande.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo ) == 0 )
	{
		debug << indent << "Unable to start new process : " << (int) GetLastError() << endl;
		CodeRet=15000;
		CodeRetour<<CodeRet;
		NVP.Add(NVP_CodeRetour, CodeRetour);
		Resp.Redirect(Req,NVP_Url,NVP);
		DB.Rollback();
		return true;
	}
	else
		cpid = (int) OpenProcess(STANDARD_RIGHTS_REQUIRED, false, pInfo.dwProcessId) ;

	// le pid récupéré de cette fonction permet d'attendre la fin dudit process, comme ceci :

	fpid = _cwait( NULL, cpid, _WAIT_CHILD );		// attente de la fin du programme

	debug << indent << "Pid Child Process = " << cpid << endl;
	debug << indent << "fpid = " << fpid << endl;


#else

	pid_t	pid;

	/*
	cpid = system(LigneDeCommande.c_str());

	// le pid récupéré de cette fonction permet d'attendre la fin dudit process, comme ceci :
	// fpid = waitpid( cpid, NULL, NULL );		// attente de la fin du programme

	debug << indent << "Pid Child Process = " << cpid << endl;
	debug << indent << "fpid = " << fpid << endl;
	*/

	switch (pid=fork())
	{
		case -1:
			debug << "Erreur sur l'appel du fils"<<endl;
			break;

		case 0:
			debug << "Appel du fils"<<endl;
			cpid = system(LigneDeCommande.c_str());

			// le pid récupéré de cette fonction permet d'attendre la fin dudit process, comme ceci :
			fpid = waitpid( cpid, NULL, NULL );		// attente de la fin du programme

			debug << indent << "pid du fork = " << (long) pid << endl;
			debug << indent << "Pid Child Process (system) = " << cpid << endl;
			debug << indent << "fpid = " << (long) fpid << endl;

			exit(0);
			break;

		default:
			debug << "Recuperation de l'id"<<endl;
	}

#endif

	LigneDeCommande.Clean();

	if ( environment != NULL)
		delete environment;

	// Ce cas n'arrive jamais (cf plus haut).
	// Si la langue a été oubliée dans le fichier de Content, on met la langue du marchand principal. Tant pis pour eux.
	// On ne teste pas les erreurs de données présentes dans leur fichier.
	if (LANGUE=="")
		CodeRet=100;

	CodeRetour<<CodeRet;
	NVP.Add(NVP_CodeRetour, CodeRetour);

	// Back to function manager
	NVP.Add(NVP_CONVERTIR,CONVERTIR);

	NVP.Add(NVP_LANGUE, LANGUE);
	NVP.Add(NVP_CHEMINOUT, CHEMINOUT);
	Resp.Redirect(Req,NVP_Url,NVP);
	return true;
}
