/******************************************************************************
 * ICAR_Library
 *
 * File : ImageBase.cpp
 *
 * Description : Implementation of base class containing some basic
 * functionalities for images
 *
 * Author : Mickael Pinto
 *
 * Mail : mickael.pinto\live.fr
 *
 * Date : Octobre 2012
 *
 *******************************************************************************/
#include "../../../include/model/image/ImageBase.hpp"

ImageBase::ImageBase(void)
{
	isValid = false;
	init();
}

ImageBase::ImageBase(const ImageBase &other)
{
	isValid = false;
	init();

	color = other.color;
	height = other.height;
	width = other.width;
	nTaille = other.nTaille;
	isValid = other.isValid;

	if (nTaille == 0)
		return;

	allocation_tableau(data, OCTET, nTaille);
	dataD = (double *)malloc(sizeof(double) * nTaille);
	isValid = true;

	for (int i = 0; i < nTaille; ++i)
	{
		data[i] = other.data[i];
		dataD[i] = other.dataD[i];
	}
}

ImageBase::ImageBase(int imWidth, int imHeight, bool isColor)
{
	isValid = false;
	init();

	color = isColor;
	height = imHeight;
	width = imWidth;
	nTaille = height * width * (color ? 3 : 1);

	if (nTaille == 0)
		return;

	allocation_tableau(data, OCTET, nTaille);
	dataD = (double *)malloc(sizeof(double) * nTaille);
	isValid = true;
}

ImageBase::~ImageBase(void)
{
	reset();
}

void ImageBase::init()
{
	if (isValid)
	{
		free(data);
		free(dataD);
	}

	data = 0;
	dataD = 0;
	height = width = nTaille = 0;
	isValid = false;
}

void ImageBase::reset()
{
	if (isValid)
	{
		free(data);
		free(dataD);
	}
	isValid = false;
}

void ImageBase::load(char *filename)
{
	init();

	int l = strlen(filename);

	if (l <= 4) // Le fichier ne peut pas etre que ".pgm" ou ".ppm"
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que '.pgm' ou '.ppm'");
		exit(0);
	}

	int nbPixel = 0;

	if (strcmp(filename + l - 3, "pgm") == 0) // L'image est en niveau de gris
	{
		color = false;
		img_pgm.lire_nb_lignes_colonnes_image_p(filename, &height, &width);
		nbPixel = height * width;

		nTaille = nbPixel;
		allocation_tableau(data, OCTET, nTaille);
		img_pgm.lire_image_p(filename, data, nbPixel);
	}
	else if (strcmp(filename + l - 3, "ppm") == 0) // L'image est en couleur
	{
		color = true;
		img_ppm.lire_nb_lignes_colonnes_image_p(filename, &height, &width);
		nbPixel = height * width;

		nTaille = nbPixel * 3;
		allocation_tableau(data, OCTET, nTaille);
		img_ppm.lire_image_p(filename, data, nbPixel);
	}
	else
	{
		printf("Chargement de l'image impossible : Le nom de fichier n'est pas conforme, il doit comporter l'extension, et celle ci ne peut �tre que .pgm ou .ppm");
		exit(0);
	}

	dataD = (double *)malloc(sizeof(double) * nTaille);

	isValid = true;
}

bool ImageBase::save(char *filename)
{
	if (!isValid)
	{
		printf("Sauvegarde de l'image impossible : L'image courante n'est pas valide");
		exit(0);
	}

	if (color)
		img_ppm.ecrire_image_p(filename, data, height, width);
	else
		img_pgm.ecrire_image_p(filename, data, height, width);

	return true;
}

ImageBase *ImageBase::getPlan(PLAN plan)
{
	if (!isValid || !color)
		return 0;

	ImageBase *greyIm = new ImageBase(width, height, false);

	switch (plan)
	{
	case PLAN_R:
		img_ppm.planR(greyIm->data, data, height * width);
		break;
	case PLAN_G:
		img_ppm.planV(greyIm->data, data, height * width);
		break;
	case PLAN_B:
		img_ppm.planB(greyIm->data, data, height * width);
		break;
	default:
		printf("Il n'y a que 3 plans, les valeurs possibles ne sont donc que 'PLAN_R', 'PLAN_G', et 'PLAN_B'");
		exit(0);
		break;
	}

	return greyIm;
}

void ImageBase::copy(const ImageBase &copy)
{
	reset();

	isValid = false;
	init();

	color = copy.color;
	height = copy.height;
	width = copy.width;
	nTaille = copy.nTaille;
	isValid = copy.isValid;

	if (nTaille == 0)
		return;

	allocation_tableau(data, OCTET, nTaille);
	dataD = (double *)malloc(sizeof(double) * nTaille);
	isValid = true;

	for (int i = 0; i < nTaille; ++i)
	{
		data[i] = copy.data[i];
		dataD[i] = copy.dataD[i];
	}
}

unsigned char *ImageBase::operator[](int l)
{
	if (!isValid)
	{
		printf("L'image courante n'est pas valide");
		exit(0);
	}

	if ((!color && l >= height) || (color && l >= height * 3))
	{
		printf("L'indice se trouve en dehors des limites de l'image");
		exit(0);
	}

	return data + l * width;
}
