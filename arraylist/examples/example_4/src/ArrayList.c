#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/ArrayList.h"

// funciones privadas
int resizeUp(ArrayList* this);
int resizeDown(ArrayList* this);
int expand(ArrayList* this,int index);
int contract(ArrayList* this,int index);

#define AL_INCREMENT      10
#define AL_INITIAL_VALUE  10
//___________________

/** \brief Allocate a new arrayList with AL_INITIAL_VALUE elements.
 * \param void
 * \return ArrayList* Return (NULL) if Error [if can't allocate memory]
 *                  - (pointer to new arrayList) if ok
 */
ArrayList* al_newArrayList(void)
{
    ArrayList* this;
    ArrayList* returnAux = NULL;
    void* pElements;
    this = (ArrayList *)malloc(sizeof(ArrayList));

    if(this != NULL)
    {
        pElements = malloc(sizeof(void *)*AL_INITIAL_VALUE );
        if(pElements != NULL)
        {
            this->size=0;
            this->pElements=pElements;
            this->reservedSize=AL_INITIAL_VALUE;
            this->add=al_add;
            this->len=al_len;
            this->set=al_set;
            this->remove=al_remove;
            this->clear=al_clear;
            this->clone=al_clone;
            this->get=al_get;
            this->contains=al_contains;
            this->push=al_push;
            this->indexOf=al_indexOf;
            this->isEmpty=al_isEmpty;
            this->pop=al_pop;
            this->subList=al_subList;
            this->containsAll=al_containsAll;
            this->deleteArrayList = al_deleteArrayList;
            this->sort = al_sort;
            returnAux = this;
        }
        else
        {
            free(this);
        }
    }

    return returnAux;
}


/** \brief  Add an element to arrayList and if is
 *          nessesary resize the array
 * \param pList ArrayList* Pointer to arrayList
 * \param pElement void* Pointer to element
 * \return int Return (-1) if Error [pList or pElement are NULL pointer] - (0) if Ok
 *
 */
int al_add(ArrayList* this, void* pElement)
{
    int returnAux = -1;
    int sinMemoria = 0;
    int indice;

    if(this != NULL && pElement != NULL)
    {
        if(this->size == this->reservedSize) //Tengo que expandir la lista
        {
            sinMemoria = resizeUp(this);
        }

        if(sinMemoria == 0)
        {
            indice = this->size;
            this->size++;
            if(al_set(this, indice, pElement) < 0) //Hubo error al guardar el dato
            {
                this->size--; //Vuelvo el tamaño a su valor original
            }
            else //OK
            {
                returnAux = 0;
            }
            //*(this->pElements + this->size) = pElement;
            //this->pElements[this->size] = pElement;
        }
    }

    return returnAux;
}

/** \brief  Delete arrayList
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList is NULL pointer] - (0) if Ok
 *
 */
int al_deleteArrayList(ArrayList* this)
{
    int returnAux = -1;

    if(this != NULL)
    {
        if(this->pElements != NULL)
        {
            free(this->pElements);
        }
        //al_clear(this);
        free(this);
        returnAux = 0;
    }

    return returnAux;
}

/** \brief  Delete arrayList
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return length of array or (-1) if Error [pList is NULL pointer]
 *
 */
int al_len(ArrayList* this)
{
    int returnAux = -1;

    if(this != NULL)
    {
        returnAux = this->size;
    }

    return returnAux;
}


/** \brief  Get an element by index
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \return void* Return (NULL) if Error [pList is NULL pointer or invalid index] - (Pointer to element) if Ok
 *
 */
void* al_get(ArrayList* this, int index)
{
    void* returnAux = NULL;

    if(this != NULL)
    {
        if(index >= 0 && index < al_len(this))
        {
            returnAux = *(this->pElements + index);
        }
    }

    return returnAux;
}


/** \brief  Find if pList contains at least one element pElement
 * \param pList ArrayList* Pointer to arrayList
 * \param pElement void* Pointer to element
 * \return int Return (-1) if Error [pList or pElement are NULL pointer]
 *                  - ( 0) if Ok but not found a element
 *                  - ( 1) if this list contains at least one element pElement
 *
 */
int al_contains(ArrayList* this, void* pElement)
{
    int returnAux = -1;

    if(this != NULL && pElement != NULL)
    {
        returnAux = 0;

        if(al_indexOf(this, pElement) >= 0)
        {
            returnAux = 1;
        }
    }

    return returnAux;
}


/** \brief  Set a element in pList at index position
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \param pElement void* Pointer to element
 * \return int Return (-1) if Error [pList or pElement are NULL pointer or invalid index]
 *                  - ( 0) if Ok
 *
 */
int al_set(ArrayList* this, int index,void* pElement)
{
    int returnAux = -1;
    int len = al_len(this);

    if(this != NULL && pElement != NULL && index >= 0 && index <= len)
    {
        if(index == len)
        {
            returnAux = al_add(this, pElement);
        }
        else
        {
            *(this->pElements + index) = pElement;
            returnAux = 0;
        }
    }

    return returnAux;
}


/** \brief  Remove an element by index
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \return int Return (-1) if Error [pList is NULL pointer or invalid index]
 *                  - ( 0) if Ok
 */
int al_remove(ArrayList* this,int index)
{
    int returnAux = -1;

    if(this != NULL && index >= 0 && index < al_len(this))
    {
        returnAux = contract(this, index);
        if(returnAux == 0)
        {
            //Libero memoria si el size tiene una distancia mayor o igual a AL_INCREMENT con respecto a reservedSize
            resizeDown(this);
        }
    }

    return returnAux;
}



/** \brief Removes all of the elements from this list
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList is NULL pointer]
 *                  - ( 0) if Ok
 */
int al_clear(ArrayList* this)
{
    int returnAux = -1;

    if(this != NULL)
    {
        this->size = 0;

        //Libero memoria al tamaño inicial definido
        //this->pElements = (void**)realloc(this->pElements, sizeof(void*) * AL_INITIAL_VALUE);
        //this->reservedSize = AL_INITIAL_VALUE;
        returnAux = resizeDown(this);
    }

    return returnAux;
}



/** \brief Returns an array containing all of the elements in this list in proper sequence
 * \param pList ArrayList* Pointer to arrayList
 * \return ArrayList* Return  (NULL) if Error [pList is NULL pointer]
 *                          - (New array) if Ok
 */
ArrayList* al_clone(ArrayList* this)
{
    ArrayList* returnAux = NULL;
    int i;
    void* pElement;

    if(this != NULL)
    {
        returnAux = al_newArrayList();
        for(i = 0; i < al_len(this); i++)
        {
            pElement = al_get(this, i);
            if(pElement == NULL || al_add(returnAux, pElement) < 0) //Si hay error vuelvo a nulificar returnAux
            {
                al_deleteArrayList(returnAux);
                returnAux = NULL;
                break;
            }
        }
        /*returnAux->pElements = this->pElements;
        returnAux->size = this->size;
        returnAux->reservedSize = this->reservedSize;*/
    }

    return returnAux;
}




/** \brief Inserts the element at the specified position
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \param pElement void* Pointer to element
 * \return int Return (-1) if Error [pList or pElement are NULL pointer or invalid index]
 *                  - ( 0) if Ok
 */
int al_push(ArrayList* this, int index, void* pElement)
{
    int returnAux = -1;
    int len = al_len(this);

    if(this != NULL && pElement != NULL && index >= 0 && index <= len)
    {
        //Si el elemento es el último, llamo a al_add
        if(index == len)
        {
            returnAux = al_add(this, pElement);
        }
        else
        {
            if(expand(this, index) == 0)
            {
                returnAux = al_set(this, index, pElement);
            }
        }
    }

    return returnAux;
}



/** \brief Returns the index of the first occurrence of the specified element
 * \param pList ArrayList* Pointer to arrayList
 * \param pElement void* Pointer to element
 * \return int Return (-1) if Error [pList or pElement are NULL pointer] - (index to element) if Ok
 */
int al_indexOf(ArrayList* this, void* pElement)
{
    int returnAux = -1;
    int i;

    if(this != NULL && pElement != NULL)
    {
        for(i = 0; i < al_len(this); i++)
        {
            if(al_get(this, i) == pElement)
            {
                returnAux = i;
                break;
            }
        }
    }

    return returnAux;
}



/** \brief Returns true if this list contains no elements.
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList is NULL pointer] - (0) if Not Empty - (1) if is Empty
 */
int al_isEmpty(ArrayList* this)
{
    int returnAux = -1;

    if(this != NULL)
    {
        returnAux = 0;
        if(al_len(this) == 0)
        {
            returnAux = 1;
        }
    }

    return returnAux;
}




/** \brief Remove the item at the given position in the list, and return it.
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \return int Return (NULL) if Error [pList is NULL pointer or invalid index]
 *                  - ( element pointer) if Ok
 */
void* al_pop(ArrayList* this,int index)
{
    void* returnAux = NULL;

    if(this != NULL && index >= 0 && index < this->size)
    {
        returnAux = al_get(this, index);

        if(al_remove(this, index) < 0) //Hubo error al remover el item
        {
            returnAux = NULL;
        }
    }

    return returnAux;
}


/** \brief Returns a new arrayList with a portion of pList between the specified
 *         fromIndex, inclusive, and toIndex, exclusive.
 * \param pList ArrayList* Pointer to arrayList
 * \param from int Initial index of the element (inclusive)
 * \param to int Final index of the element (exclusive)
 * \return int Return (NULL) if Error [pList is NULL pointer or invalid 'from' or invalid 'to']
 *                  - ( pointer to new array) if Ok
 */
ArrayList* al_subList(ArrayList* this,int from,int to)
{
    ArrayList* returnAux = NULL;
    int i;

    if(this != NULL && from >= 0 && to <= this->size && from < to)
    {
        returnAux = al_newArrayList();
        for(i = from; i < to; i++)
        {
            if(al_add(returnAux, al_get(this, i)) < 0) //Si hay error vuelvo a nulificar returnAux
            {
                al_deleteArrayList(returnAux);
                returnAux = NULL;
                break;
            }
        }
    }

    return returnAux ;
}





/** \brief Returns true if pList list contains all of the elements of pList2
 * \param pList ArrayList* Pointer to arrayList
 * \param pList2 ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList or pList2 are NULL pointer]
 *                  - (0) if Not contains All - (1) if is contains All
 */
int al_containsAll(ArrayList* this,ArrayList* this2)
{
    int returnAux = -1;
    int i;
    int cantidadHallada = 0;

    if(this != NULL && this2 != NULL)
    {
        returnAux = 0;

        for(i = 0; i < this2->size; i++)
        {
            if(al_indexOf(this, al_get(this2, i)) >= 0)
            {
                cantidadHallada++;
            }
        }

        if(cantidadHallada == al_len(this2)) //Si todos los elementos de this2 fueron hallados en this
        {
            returnAux = 1;
        }
    }

    return returnAux;
}

/** \brief Sorts objects of list, use compare pFunc
 * \param pList ArrayList* Pointer to arrayList
 * \param pFunc (*pFunc) Pointer to fuction to compare elements of arrayList
 * \param order int  [1] indicate UP - [0] indicate DOWN
 * \return int Return (-1) if Error [pList or pFunc are NULL pointer]
 *                  - (0) if ok
 */
int al_sort(ArrayList* this, int (*pFunc)(void* ,void*), int order)
{
    int returnAux = -1;
    int i;
    int j;
    void* elementoI;
    void* elementoJ;
    //void* aux;
    int huboError = 0;

    if(this != NULL && pFunc != NULL && (order == 0 || order == 1))
    {
        for(i = 0; i < this->size - 1; i++)
        {
            for(j = i + 1; j < this->size; j++)
            {
                elementoI = al_get(this, i);
                elementoJ = al_get(this, j);
                if((pFunc(elementoI, elementoJ) == -1 && order == 0) || (pFunc(elementoI, elementoJ) == 1 && order == 1))
                {
                    //aux = elementoI;
                    if(al_set(this, i, elementoJ) < 0)
                    {
                        huboError = 1;
                        break;
                    }
                    if(al_set(this, j, elementoI) < 0)
                    {
                        huboError = 1;
                        break;
                    }
                }
            }

            if(huboError == 1)
            {
                break;
            }
        }

        if(huboError == 0)
        {
            returnAux = 0;
        }
    }

    return returnAux;
}


/** \brief Increment the number of elements in pList in AL_INCREMENT elements.
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList is NULL pointer or if can't allocate memory]
 *                  - (0) if ok
 */
int resizeUp(ArrayList* this)
{
    int returnAux = -1;
    void** pTemp;

    if(this != NULL)
    {
        pTemp = (void**)realloc(this->pElements, sizeof(void*) * (this->reservedSize + AL_INCREMENT));

        if(pTemp != NULL)
        {
            this->pElements = pTemp;
            this->reservedSize = this->reservedSize + AL_INCREMENT;
            returnAux = 0;
        }
    }

    return returnAux;
}


/** \brief Decrement the number of elements in pList in AL_INCREMENT elements.
 * \param pList ArrayList* Pointer to arrayList
 * \return int Return (-1) if Error [pList is NULL pointer]
 *                  - (0) if ok
 */
int resizeDown(ArrayList* this)
{
    int returnAux = -1;
    int len = al_len(this);

    if(this != NULL)
    {
        if(len + AL_INCREMENT <= this->reservedSize)
        {
            this->pElements = (void**)realloc(this->pElements, sizeof(void*) * (len + AL_INCREMENT));
            this->reservedSize = len + AL_INCREMENT;
            returnAux = 0;
        }
    }

    return returnAux;
}

/** \brief  Expand an array list
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \return int Return (-1) if Error [pList is NULL pointer or invalid index]
 *                  - ( 0) if Ok
 */
int expand(ArrayList* this,int index)
{
    int returnAux = -1;
    int i;
    int huboError = 0;

    if(this != NULL && index >= 0 && index < this->size)
    {
        for(i = this->size; i > index; i--)
        {
            if(i == this->size)
            {
                if(al_add(this, al_get(this, i - 1)) < 0) //Hubo error al guardar el dato
                {
                    huboError = 1;
                    break;
                }
            }
            else
            {
                //*(this->pElements + i) = *(this->pElements + i - 1);
                if(al_set(this, i, al_get(this, i - 1)) < 0) //Hubo error al guardar el dato
                {
                    huboError = 1;
                    break;
                }
            }
        }

        if(huboError == 0)
        {
            returnAux = 0;
        }
    }

    return returnAux;
}

/** \brief  Contract an array list
 * \param pList ArrayList* Pointer to arrayList
 * \param index int Index of the element
 * \return int Return (-1) if Error [pList is NULL pointer or invalid index]
 *                  - ( 0) if Ok
 */
int contract(ArrayList* this,int index)
{
    int returnAux = -1;
    int i;
    int len = al_len(this);

    if(this != NULL && index >= 0 && index < len)
    {
        for(i = index; i < (len - 1); i++)
        {
            al_set(this, i, al_get(this, i + 1));
        }

        this->size--;
        returnAux = 0;
    }

    return returnAux;
}
